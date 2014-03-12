// generic_system_coordinator.h

#ifndef __GENERIC_SYSTEM_COORDINATOR_H
#define __GENERIC_SYSTEM_COORDINATOR_H

#include "rtp_interface/systems/rtp_system.h"
#include "rtp_interface/systems/rtp_agent.h"
#include "rtp_interface/systems/rtp_interactive_agent.h"

#include "system_coordinator.h"
#include "wt_system_widgets/system_display_widget.h"
#include "wt_system_widgets/properties_chart_widget.h"

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <chrono>


class generic_sys_coordinator: public system_coordinator
{
public:
	typedef boost::shared_ptr< i_agent >			controller_ptr_t;

	enum {
		UpdateFrequency = 60,	// Per second
		RenderPeriod = 3,		// Per update (1 = Render every update, 2 = Render every second update, etc)
		ChartPeriod = 60,		// As above
	};

	typedef sys_display_widget						widget_t;
	typedef properties_chart_widget					chart_widget_t;

public:
	generic_sys_coordinator(i_system* sys):
		m_sys(sys),
		m_avg_update_period(0.0f),
		m_timer(m_io)
	{
		m_app = Wt::WApplication::instance();
		m_request_termination = false;
	}

	~generic_sys_coordinator()
	{
		terminate_system();
	}

	virtual std::pair< Wt::WWidget*, Wt::WWidget* > initialize()
	{
		boost::lock_guard< boost::mutex > guard(m_sys_mutex);

		m_widget = new sys_display_widget(m_sys_mutex);
		m_widget->set_drawer(m_sys->get_drawer());
		m_widget->interactive_input_sig().connect(this, &generic_sys_coordinator::on_widget_interaction);

		m_chart_widget = new chart_widget_t();
		m_chart_widget->reset(m_sys->get_state_properties());

		return std::pair< Wt::WWidget*, Wt::WWidget* >(m_widget, m_chart_widget);
	}

	void set_agent_controllers(boost::shared_ptr< i_agent > a)
	{
		boost::lock_guard< boost::mutex > guard(m_sys_mutex);

		assert(a);
		m_sys->clear_agents();
		boost::optional< agent_id_t > id1 = m_sys->register_agent(a);
		assert(id1);
		m_controllers[*id1] = controller_ptr_t(a);
	}

	virtual void restart()
	{
		// TODO: If restart button is hit while system is still going, we will wait for update thread while update thread may enter
		// Wt UpdateLock, resulting in deadlock...
		terminate_system();
		m_request_termination = false;

		rgen_t rgen;
		rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
		
		m_sys->set_state(m_sys->generate_initial_state(rgen));

		m_widget->enable_interaction(true);	// TODO:
		m_chart_widget->clear_content();

		m_step = 0;
		m_avg_update_period = 0.0f;
		update_widgets(true);

		m_io.reset();
		m_timer.expires_from_now(boost::posix_time::milliseconds(1000.0f / UpdateFrequency));
		m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_update, this));
		boost::thread t(boost::bind(&boost::asio::io_service::run, &m_io));
		m_update_thread.swap(t);

//		Wt::WTimer::singleShot(1000.0f / UpdateFrequency, this, &generic_sys_coordinator::on_update);
	}

	void update_widgets(bool force_update)
	{
		if(force_update || (m_step % RenderPeriod == 0))
		{
			// TODO: Is this safe, given that Wt event loop system could potentially invoke a method of m_widget (I guess) ?
			m_widget->update();
		}

		if(force_update || (m_step % ChartPeriod == 0))
		{
			boost::lock_guard< boost::mutex > guard(m_sys_mutex);
			m_chart_widget->append_data(m_sys->get_state_property_values());
		}
	}

private:
	void terminate_system()
	{
		// TODO: threadsafe way to interrupt currently updating system, rather than having to wait for it to finish

		m_request_termination = true;
		m_update_thread.join();
	}

	void on_widget_interaction(interactive_input const& input)
	{
//		controller_ptr_t controller = m_controllers.begin()->second;	// TODO: For now assuming only one agent

		// Lock since system update thread may be accessing controller from within system method
		boost::lock_guard< boost::mutex > guard(m_sys_mutex);
//		boost::dynamic_pointer_cast< i_interactive_agent >(controller)->register_input(idx, down);
		m_sys->register_interactive_input(input);
	}

	void on_update()
	{
		// TODO: Per comment above, this is not sufficient since Wt event loop thread may enter join() while we are past this
		// if statement, but before the update lock...
		if(m_request_termination)
		{
			return;
		}

		auto tm1 = std::chrono::high_resolution_clock::now();

		if(m_step > 0)
		{
			auto period = std::chrono::duration_cast< std::chrono::duration< float > >(tm1 - m_last_update_tm);
			m_avg_update_period = ((m_step - 1) * m_avg_update_period + period.count()) / m_step;
		}
		m_last_update_tm = tm1;

		bool result;
		{
			boost::lock_guard< boost::mutex > guard(m_sys_mutex);
			result = m_sys->update(nullptr);
		}
		++m_step;

		auto tm2 = std::chrono::high_resolution_clock::now();
		auto system_update_duration = tm2 - tm1;

		{
			Wt::WApplication::UpdateLock lock(m_app);
			if(lock)
			{
				update_widgets(!result);
				m_app->triggerUpdate();
			}
			else
			{
				result = false;
			}
		}

		auto tm3 = std::chrono::high_resolution_clock::now();
		auto widget_update_duration = tm3 - tm2;

		if(result)
		{
			m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(1000.0f / UpdateFrequency));
			m_timer.async_wait(boost::bind(&generic_sys_coordinator::on_update, this));

//			Wt::WTimer::singleShot(1000.0f / UpdateFrequency, this, &generic_sys_coordinator::on_update);
		}
	}

private:
	i_system* m_sys;
	widget_t* m_widget;
	chart_widget_t* m_chart_widget;
	unsigned int m_step;

	boost::asio::io_service m_io;
	boost::thread m_update_thread;
	boost::asio::deadline_timer m_timer;
	boost::mutex m_sys_mutex;
	Wt::WApplication* m_app;
	bool m_request_termination;

	//
	std::chrono::high_resolution_clock::time_point m_last_update_tm;
	float m_avg_update_period;
	//

	std::map< agent_id_t, controller_ptr_t > m_controllers;
};


#endif

