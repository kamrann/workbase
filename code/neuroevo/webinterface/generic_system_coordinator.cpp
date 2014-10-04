// generic_system_coordinator.cpp

#include "generic_system_coordinator.h"
#include "wt_system_widgets/properties_table_widget.h"
#include "wt_system_widgets/properties_chart_widget.h"

#include <Wt/WServer>


// todo:
auto const AnimationFreq = 10.0;
auto const NonRealtimeFreq = 10.0;	// TODO: Decide what is meant by realtime/non-realtime, and how that
	// relates to drawer::is_animated()
//

generic_sys_coordinator::generic_sys_coordinator(
	std::unique_ptr< rtp::i_system_factory > sys_factory,
	rtp::agents_data agents):
	m_sys_factory(std::move(sys_factory)),
	m_agents_spec(std::move(agents)),
	m_avg_update_period(0.0f),
	m_timer(m_io)
{
	m_app = Wt::WApplication::instance();
	m_request_termination = false;
}

generic_sys_coordinator::~generic_sys_coordinator()
{
	terminate_system();
}

std::pair< Wt::WWidget*, Wt::WWidget* > generic_sys_coordinator::initialize()
{
	std::lock_guard< std::mutex > guard(m_sys_mutex);

	m_widget = new sys_display_widget(m_sys_mutex);
	//m_widget->set_drawer(m_sys->get_drawer());
	m_widget->interactive_input_sig().connect(this, &generic_sys_coordinator::on_widget_interaction);

	// TODO: This seems hacky, but don't want the selected series to be reset on restart,
	// since the system type is unchanged and therefore so are the properties
	auto temp_sys = m_sys_factory->create_system();
	temp_sys->initialize();
	for(auto const& agent : m_agents_spec.agents)
	{
		auto agent_id = temp_sys->register_agent(agent.spec);// , std::move(controller));
		auto controller = std::unique_ptr< rtp::i_controller >(
			m_agents_spec.controller_factories[agent.controller_id]->create(&temp_sys->get_agent(agent_id))
			);
		temp_sys->register_agent_controller(agent_id, std::move(controller));
	}

	Wt::WWidget* props_wt_wgt = nullptr;
	if(temp_sys->is_instantaneous())
	{
		auto wgt = new properties_table_widget{};
		m_props_widget = wgt;
		props_wt_wgt = wgt;
	}
	else
	{
		auto wgt = new properties_chart_widget{};
		m_props_widget = wgt;
		props_wt_wgt = wgt;
	}
	m_props_widget->reset(temp_sys->get_state_properties());

	return std::pair< Wt::WWidget*, Wt::WWidget* >(m_widget, props_wt_wgt);
}

void generic_sys_coordinator::cancel()
{
	terminate_system();
}

void generic_sys_coordinator::restart()
{
	terminate_system();

	m_sys = m_sys_factory->create_system();
	m_sys->initialize();

	rtp::interactive_input_set all_inputs;
	for(auto const& agent : m_agents_spec.agents)
	{
		auto agent_id = m_sys->register_agent(agent.spec);
		auto& factory = m_agents_spec.controller_factories[agent.controller_id];
		auto controller = std::unique_ptr< rtp::i_controller >(
			factory->create(&m_sys->get_agent(agent_id))
			);

		if(factory->is_interactive())
		{
			auto interactive = static_cast<rtp::i_interactive_controller*>(controller.get());
			auto const& required_inputs = interactive->get_required_interactive_inputs();
			all_inputs.insert(required_inputs.begin(), required_inputs.end());
		}

		m_sys->register_agent_controller(agent_id, std::move(controller));
	}
	/////

	m_widget->set_drawer(m_sys->get_drawer());
	m_widget->enable_interaction(all_inputs);

	m_props_widget->clear_content();
	//m_props_widget->reset(m_sys->get_state_properties());

	m_system_st = SystemState::Active;
	m_step = 0;
	m_avg_update_period = 0.0f;

	// TODO: Set focus????  m_widget->clicked().emit(Wt::WMouseEvent());

	auto uinf = m_sys->get_update_info();

	bool instant = m_sys->is_instantaneous();
	bool realtime = uinf.type == rtp::i_system::update_info::Type::Realtime;
	bool animated = !realtime && m_widget->get_drawer()->is_animated();
	auto delay = instant ?
		0.0 :
		(
		realtime ?
		(1.0 / uinf.frequency) :
		(
		animated ?
		(1.0 / AnimationFreq) :
		(1.0 / NonRealtimeFreq)
		)
		);

	m_io.reset();
	m_timer.expires_from_now(boost::posix_time::milliseconds(1000.0f * delay));
	if(animated)
	{
		auto anim_drawer = static_cast<rtp::animated_system_drawer*>(m_widget->get_drawer());
		anim_drawer->start();

		m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_animation_update, this));
	}
	else
	{
		m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_system_update, this));
	}
	std::thread t(boost::bind(&boost::asio::io_service::run, &m_io));
	m_update_thread.swap(t);

	m_ready_to_display = !instant;
	update_widgets();
}

void generic_sys_coordinator::register_system_finish_callback(std::function< void() > cb)
{
	m_finish_cb = [this, cb]
	{
		Wt::WServer::instance()->post(m_app->sessionId(), cb);
	};
}

void generic_sys_coordinator::update_widgets()//bool force_update)
{
	bool ready = m_ready_to_display;	// Make a copy for the lambda
	Wt::WServer::instance()->post(m_app->sessionId(), [this, ready]()
	{
		m_widget->set_ready(ready);
		m_widget->update();

		if(ready)
		{
			std::lock_guard< std::mutex > guard(m_sys_mutex);
			m_props_widget->register_data(m_sys->get_state_property_values());
		}
		else
		{
			m_props_widget->clear_content();
		}

		m_app->triggerUpdate();
	});

	/*
	if(force_update || (m_step % RenderPeriod == 0))
	{
		// TODO: Is this safe, given that Wt event loop system could potentially invoke a method of m_widget (I guess) ?
		m_widget->update();
	}

	if(force_update || (m_step % ChartPeriod == 0))
	{
		std::lock_guard< std::mutex > guard(m_sys_mutex);
		m_chart_widget->append_data(m_sys->get_state_property_values());
	}
	*/
}

void generic_sys_coordinator::terminate_system()
{
	if(m_update_thread.joinable())
	{
		m_request_termination = true;
		m_update_thread.join();
		m_request_termination = false;
	}
}

void generic_sys_coordinator::on_widget_interaction(rtp::interactive_input const& input)
{
	// Lock since system update thread may be accessing controller from within system method
	std::lock_guard< std::mutex > guard(m_sys_mutex);

	m_sys->register_interactive_input(input);
}

void generic_sys_coordinator::on_animation_update()
{
	if(m_request_termination)
	{
		return;
	}

	auto drawer = static_cast< rtp::animated_system_drawer* >(m_widget->get_drawer());

	// Advance animation state
	auto result = drawer->advance();

	switch(result)
	{
		case rtp::animated_system_drawer::Result::Ongoing:
		// Draw
		update_widgets();

		// And schedule next frame
		m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(1000.0f / AnimationFreq));
		m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_animation_update, this));
		break;

		case rtp::animated_system_drawer::Result::Finished:
		{
			switch(m_system_st)
			{
				case SystemState::Active:
//				m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(1000.0f / AnimationFreq /* TODO: immediate??? */));
//				m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_system_update, this));
				on_system_update();
				break;

				case SystemState::Completed:
				// All done
				if(m_finish_cb)
				{
					m_finish_cb();
				}
				break;
			}
		}
		break;
	}
}

void generic_sys_coordinator::time_check()
{
	auto tm1 = std::chrono::high_resolution_clock::now();
	if(m_step > 0)
	{
		auto period = std::chrono::duration_cast<std::chrono::duration< float >>(tm1 - m_last_update_tm);
		m_avg_update_period = ((m_step - 1) * m_avg_update_period + period.count()) / m_step;
	}
	m_last_update_tm = tm1;
}

void generic_sys_coordinator::on_system_update()
{
	if(m_request_termination)
	{
		return;
	}

	time_check();

	bool result;
	{
		std::lock_guard< std::mutex > guard(m_sys_mutex);
		result = m_sys->update(nullptr);
		m_system_st = result ? SystemState::Active : SystemState::Completed;
	}
	++m_step;

	m_ready_to_display = true;

	auto uinf = m_sys->get_update_info();
	if(uinf.type == rtp::i_system::update_info::Type::Realtime)
	{
		if(m_system_st == SystemState::Completed || (m_step % RenderPeriod == 0))
		{
			update_widgets();
		}

		if(m_system_st == SystemState::Active)
		{
			m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(1000.0f / uinf.frequency));
			m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_system_update, this));
		}
		else if(m_finish_cb)
		{
			m_finish_cb();
		}
	}
	else
	{
		if(m_widget->get_drawer()->is_animated())
		{
			auto anim_drawer = static_cast<rtp::animated_system_drawer*>(m_widget->get_drawer());
			anim_drawer->start();

			// Initiate animation sequence
			m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(1000.0f / AnimationFreq));
			m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_animation_update, this));
		}
		else if(m_system_st == SystemState::Active)
		{
			m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(1000.0f / NonRealtimeFreq));
			m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_system_update, this));
		}
		else if(m_finish_cb)
		{
			m_finish_cb();
		}

		// Draw
		update_widgets();
	}
}


