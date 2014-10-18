// sys_controller.cpp

#include "sys_controller.h"
#include "Wt displays/wt_server.h"

#include <thread>


namespace sys_control {

	controller::controller(
		std::shared_ptr< sys::i_system_defn > sys_defn,
		sys::system_ptr sys,
		sys::state_value_id_list update_vals,
		sys::state_value_id_list result_vals,
		std::ostream& os):
		m_sys_defn{ sys_defn },
		m_system{ std::move(sys) },
		m_update_vals{ std::move(update_vals) },
		m_result_vals{ std::move(result_vals) },
		m_os{ os }
	{
		m_sys_rseed = 0;
	}

	void controller::start()
	{
		m_cmds_waiting = 0;
		m_io_work = std::make_unique< asio::io_service::work >(m_io_service);

		// Auto initialize system for first run
		m_io_service.post([this]
		{
			reset_system(boost::none);
		});

		m_io_service.run();
	}

	void controller::post(std::function< void() > fn)
	{
		// Atomically increment waiting count and post command processing function to io service
		{
			std::lock_guard< std::mutex > lock_{ m_cmd_mx };

			++m_cmds_waiting;

			m_io_service.post([this, fn]
			{
				// Decrement waiting count
				{
					std::lock_guard< std::mutex > lock_{ m_cmd_mx };
					--m_cmds_waiting;
				}

				// Invoke command function
				fn();

				// If no other commands waiting and in run mode, resume system
				{
					std::lock_guard< std::mutex > lock_{ m_cmd_mx };
					if(m_cmds_waiting == 0 && m_mode == Mode::Running)
					{
						m_io_service.post([this]
						{
							do_run();
						});
					}
				}
			});
		}
	}

	void controller::terminate()
	{
		m_io_work.reset();
	}

	
	bool controller::update_frame(frame_update_cfg const& cfg)
	{
		bool active;
		{
			std::lock_guard< std::mutex > lock_{ m_sys_mx };

			active = m_system->update(nullptr);
		}

		if(cfg.output_updates)
		{
			output_update_values();
		}

		if(cfg.store_chart_series_data)
		{
			store_chart_series_data();
		}

		if(cfg.redraw_charts)
		{
			redraw_all_charts();
		}

		if(cfg.redraw_system)
		{
			redraw_all_system_views();
		}

		++m_frame;

		return active;
	}

	void controller::output_values(sys::state_value_id_list const& vals)
	{
		std::lock_guard< std::mutex > lock_{ m_sys_mx };

		for(auto v_id : vals)
		{
			auto bound = m_system->get_state_value_binding(v_id);
			auto value = m_system->get_state_value(bound);
			m_os << v_id.to_string() << ": " << value << std::endl;
		}
	}

	void controller::output_update_values()
	{
		if(!m_update_vals.empty())
		{
			m_os << "update state values:" << std::endl;
			output_values(m_update_vals);
		}
	}

	void controller::store_chart_series_data()
	{
		std::lock_guard< std::mutex > lock_{ m_sys_mx };

		for(auto const& entry : m_charts)
		{
			auto id = entry.first;
			auto& dd = get_display_data(id);
			auto& chart_data = boost::any_cast<chart_dd&>(dd.data);
			chart_data.x_vals.push_back(m_frame);	// TODO: Support using state value on x axis (eg time)
			for(size_t s_idx = 0; s_idx < entry.second.series.size(); ++s_idx)
			{
				auto const& series = entry.second.series[s_idx];
				auto val = m_system->get_state_value(series.sv_bound_id);
				chart_data.series[s_idx].y_vals.push_back(val);
			}
			release_display_data();
		}
	}

	void controller::redraw_all_charts()
	{
		for(auto const& entry : m_charts)
		{
			auto id = entry.first;
			update_display(id);
		}
	}

	void controller::redraw_all_system_views()
	{
		for(auto const& entry : m_drawers)
		{
			auto id = entry.first;
			update_display(id);
		}
	}

	void controller::add_chart(sys::state_value_id_list vals)
	{
		display_data dd;
		dd.type = DisplayType::Chart;
		chart_dd chart_data;
		chart_defn defn;
		{
			std::lock_guard< std::mutex > lock_{ m_sys_mx };

			for(auto const& val : vals)
			{
				chart_data.series.push_back(chart_dd::series_data{ val.to_string(), {} });
				defn.series.push_back(chart_defn::series_defn{ m_system->get_state_value_binding(val) });
			}
		}
		dd.data = chart_data;
		auto id = create_wt_display(dd);
		m_charts[id] = std::move(defn);
	}

	void controller::add_drawer()
	{
		display_data dd;
		dd.type = DisplayType::Drawer;
		drawer_dd drawer_data;
		drawer_data.mx = &m_sys_mx;
		{
			std::lock_guard< std::mutex > lock_{ m_sys_mx };
			drawer_data.drawer.reset(m_system->get_drawer().release());	//m_sys_defn->get_drawer().release();
		}
		drawer_defn defn;
		dd.data = drawer_data;
		auto id = create_wt_display(dd);
		m_drawers[id] = std::move(defn);
	}

	void controller::reset_system(boost::optional< unsigned int > rseed)
	{
		if(rseed)
		{
			m_sys_rseed = *rseed;
		}

		m_system->set_random_seed(m_sys_rseed);
		m_system->initialize();
		m_frame = 0;
		m_mode = Mode::Stopped;

		clear_all_chart_data();
		
		for(auto& drawer : m_drawers)
		{
			update_display(drawer.first);
		}
	}

	void controller::run(boost::optional< double > frame_rate)
	{
		m_run_framerate = frame_rate;
		m_mode = Mode::Running;
	}

	void controller::do_run()
	{
		frame_update_cfg cfg;
		if(!m_run_framerate)
		{
			// Disable redraws for continuous update
			cfg.redraw_charts = false;
			cfg.redraw_system = false;
		}

		bool active = true;
		while(active && !commands_waiting())
		{
			if(m_run_framerate)
			{
				auto dur = std::chrono::duration < double > { 1.0 / *m_run_framerate };
				std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(dur));
			}

			active = update_frame(cfg);
		}

		redraw_all_charts();
		redraw_all_system_views();

		if(!active)
		{
			notify_completion();
		}
	}

	void controller::stop()
	{
		m_mode = Mode::Stopped;
	}

	void controller::step(unsigned int frames)
	{
		for(size_t f = 0; f < frames; ++f)
		{
			auto active = update_frame(frame_update_cfg{});
			if(!active)
			{
				notify_completion();
				break;
			}
		}
	}

	bool controller::commands_waiting()
	{
		std::lock_guard< std::mutex > lock_{ m_cmd_mx };
		return m_cmds_waiting > 0;
	}
	/*
	bool controller::interrupted()
	{
		return m_interrupt;
	}

	void controller::interrupt()
	{
		m_interrupt = true;
	}
	*/

	void controller::notify_completion()
	{
//		std::lock_guard< std::mutex > lock_(m_cmd_mx);
		m_mode = Mode::Completed;

		m_os << "system completed" << std::endl;

		// Output result values
		if(!m_result_vals.empty())
		{
			m_os << "result state values:" << std::endl;
			output_values(m_result_vals);
		}
	}

	void controller::clear_chart_data(unsigned long id)
	{
		auto& dd = get_display_data(id);
		auto& chart_data = boost::any_cast<chart_dd&>(dd.data);
		chart_data.x_vals.clear();
		for(auto& series : chart_data.series)
		{
			series.y_vals.clear();
		}
		chart_data.reload = true;
		release_display_data();

		update_display(id);
	}

	void controller::clear_all_chart_data()
	{
		for(auto& ch : m_charts)
		{
			clear_chart_data(ch.first);
		}
	}

}

