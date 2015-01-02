// active_state.cpp

#include "active_state.h"
#include "control_fsm.h"

#include "wt_cmdline_server/wt_server.h"
#include "wt_displays/chart.h"
#include "wt_displays_ne/drawer.h"

#include "system_sim/system_defn.h"
#include "system_sim/system.h"

#include "system_sim/ui_based_controller.h"

#include "input_stream/x360_input_stream.h"


namespace sys_control {
	namespace fsm {

		active::active(my_context ctx):
			base(ctx)
		{
			reg_cmd< exit_cmd >([]()->qi::rule< clsm::iter_t, exit_cmd(), clsm::skip_t >{ return qi::lit("exit")[phx::nothing]; });
			reg_cmd< reset_cmd >(wrap_grammar< reset_cmd_parser< clsm::iter_t > >());

			auto& sc_ctx = context< system_controller >();

			// Entering active system state, construct a system from the param tree
			auto nav = ddl::navigator{ &sc_ctx.ddl_data, ddl::sd_node_ref{ sc_ctx.ddl_data.get_root() } };
			
			auto sys_type = nav["sys_type"].get().as_single_enum_str();
			auto const& sys_defn = sc_ctx.sys_defns.at(sys_type);
			sys = sys_defn->create_system(nav["sys_details"][(size_t)0]);	// TODO: [0] is hack to access condition child

			// And reset it
			sys_rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
			reset_system(boost::none);

			// Also store update state value ids
			auto update_value_names = nav["updates"].get().as_enum_str();
			for(auto const& v : update_value_names)
			{
				update_vals.push_back(sys::state_value_id::from_string(v));
			}

			// And result state value ids
			auto result_value_names = nav["results"].get().as_enum_str();
			for(auto const& v : result_value_names)
			{
				result_vals.push_back(sys::state_value_id::from_string(v));
			}


			/////////////////////////
			// TODO: hard coding input stream type
			auto in = std::make_shared< input::x360_input_stream >();
			in->connect_to_controller();
			in_strm = in;
			//
			auto cont = const_cast< sys::i_controller* >(&sys->get_agent_controller(0));	// TODO: 0 id BIG HACK!!!!
			auto as_ui_cont = dynamic_cast<sys::ui_based_controller*>(cont);	// TODO: another hack
			if(as_ui_cont)
			{
				as_ui_cont->set_input_stream(in_strm);
			}
		}

		void active::on_reset(clsm::ev_cmd< reset_cmd > const& cmd)
		{
			boost::optional< unsigned int > rseed;
			switch(cmd.seed.method)
			{
				case reset_cmd::seed_data::SeedMethod::Random:
				rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
				break;

				case reset_cmd::seed_data::SeedMethod::Fixed:
				rseed = cmd.seed.value;
				break;

				default:	// Same - leave rseed uninitialized so argument will be ignored and seed left unchanged
				break;
			}
			reset_system(rseed);
		}

		void active::on_chart(clsm::ev_cmd< chart_cmd > const& cmd)
		{
			sys::state_value_id_list ids;
			for(auto const& v : cmd.values)
			{
				ids.push_back(sys::state_value_id::from_string(v));
			}
			add_chart(ids);
		}

		void active::on_drawer(clsm::ev_cmd< drawer_cmd > const& cmd)
		{
			auto zoom = cmd.zoom ? *cmd.zoom : 1.0;
			add_drawer(zoom);
		}

		sc::result active::react(clsm::ev_cmd< reset_cmd > const& cmd)
		{
			on_reset(cmd);
			// TODO: THIS IS WRONG, on_reset() work is meaningless as active state is recreated
			// think just want to transition to paused?
			return transit< active >();
		}

		// Initialize the system
		void active::reset_system(boost::optional< unsigned int > rseed)
		{
			if(rseed)
			{
				sys_rseed = *rseed;
			}

			{
				std::lock_guard< std::mutex > lock_{ sys_mx };

				sys->set_random_seed(sys_rseed);
				sys->initialize();
			}
			frame = 0;
			complete = false;

			clear_all_chart_data();

			for(auto& drawer : drawers)
			{
				update_display(drawer.first);
			}
		}

		// Advances the system by a single update frame
		bool active::step_system(frame_update_cfg const& cfg)
		{
			in_strm->update();

			bool still_going;
			{
				std::lock_guard< std::mutex > lock_{ sys_mx };

				still_going = sys->update(nullptr);
			}

			if(cfg.output_updates)
			{
				output_state_values(update_vals);
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

			++frame;

			return still_going;
		}

		bool active::step_system(size_t frames, frame_update_cfg const& cfg)
		{
			for(size_t i = 0; i < frames; ++i)
			{
				if(!step_system(cfg))
				{
					return false;
				}
			}
			return true;
		}

		void active::output_state_values(sys::state_value_id_list const& svids)
		{
			std::lock_guard< std::mutex > lock_{ sys_mx };

			for(auto v_id : svids)
			{
				auto bound = sys->get_state_value_binding(v_id);
				auto value = sys->get_state_value(bound);
				std::stringstream ss;
				ss << v_id.to_string() << ": " << value;
				context< system_controller >().output_sink(ss.str());
			}
		}

		void active::add_chart(sys::state_value_id_list vals)
		{
			display_data dd;
			dd.type = chart_display_defn::ID;//DisplayType::Chart;
			chart_dd chart_data;
			chart_defn defn;
			{
				std::lock_guard< std::mutex > lock_{ sys_mx };

				for(auto const& val : vals)
				{
					chart_data.series.push_back(chart_dd::series_data{ val.to_string(), {} });
					defn.series.push_back(chart_defn::series_defn{ sys->get_state_value_binding(val) });
				}
			}
			dd.data = chart_data;
			auto id = create_wt_display(dd);
			charts[id] = std::move(defn);
		}

		void active::add_drawer(double zoom)
		{
			display_data dd;
			dd.type = drawer_display_defn::ID;
			drawer_dd drawer_data;
			drawer_data.mx = &sys_mx;
			{
				std::lock_guard< std::mutex > lock_{ sys_mx };

				drawer_data.drawer.reset(sys->get_drawer().release());	//m_sys_defn->get_drawer().release();

				// TODO: stream type hard coded!
//				drawer_data.in_strm = todo cast of: in_strm;
					//std::make_shared< input::wt_input_stream >();

/*				auto cont = const_cast< sys::i_controller* >(&sys->get_agent_controller(0));	// TODO: 0 id BIG HACK!!!!
				auto as_ui_cont = dynamic_cast<sys::ui_based_controller*>(cont);	// TODO: another hack
				if(as_ui_cont)
				{
					as_ui_cont->set_input_stream(//drawer_data.
						in_strm);
				}
*/
			}
			drawer_data.zoom = zoom;
			
			drawer_defn defn;
			dd.data = drawer_data;
			auto id = create_wt_display(dd);
			drawers[id] = std::move(defn);
		}

		void active::clear_chart_data(unsigned long id)
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

		void active::clear_all_chart_data()
		{
			for(auto& ch : charts)
			{
				clear_chart_data(ch.first);
			}
		}

		void active::store_chart_series_data()
		{
			std::lock_guard< std::mutex > lock_{ sys_mx };

			for(auto const& entry : charts)
			{
				auto id = entry.first;
				auto& dd = get_display_data(id);
				auto& chart_data = boost::any_cast<chart_dd&>(dd.data);
				chart_data.x_vals.push_back(frame);	// TODO: Support using state value on x axis (eg time)
				for(size_t s_idx = 0; s_idx < entry.second.series.size(); ++s_idx)
				{
					auto const& series = entry.second.series[s_idx];
					auto val = sys->get_state_value(series.sv_bound_id);
					chart_data.series[s_idx].y_vals.push_back(val);
				}
				release_display_data();
			}
		}

		void active::redraw_all_charts()
		{
			for(auto const& entry : charts)
			{
				auto id = entry.first;
				update_display(id);
			}
		}

		void active::redraw_all_system_views()
		{
			for(auto const& entry : drawers)
			{
				auto id = entry.first;
				update_display(id);
			}
		}

	}
}




