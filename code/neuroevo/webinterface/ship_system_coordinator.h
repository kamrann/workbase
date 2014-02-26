// ship_system_coordinator.h

#ifndef __SHIP_SYSTEM_COORDINATOR_H
#define __SHIP_SYSTEM_COORDINATOR_H

#include "system_coordinator.h"
#include "wt_system_widgets/ship_widget.h"
//#include "wt_system_widgets/ship_history_widget.h"

#include "rtp_interface/systems/sat/rtp_sat_system.h"

// TEMP
#include "rtp_interface/systems/sat/scenarios/full_stop/agents.h"
//

#include <Wt/WTimer>

#include <chrono>


template < WorldDimensionality dim >
class ship_coordinator: public system_coordinator
{
public:
	typedef rtp_sat::sat_system< dim >					system_t;
	typedef i_agent										agent_controller_t;
	typedef boost::shared_ptr< agent_controller_t >		controller_ptr_t;

	enum {
		FrameDuration = 50,
	};

	typedef ship_widget									widget_t;
	//typedef ship_history_widget< Dim >				history_widget_t;

public:
	ship_coordinator(i_system* sys): m_sys((rtp_sat::sat_system< dim >*)sys)
	{}

	virtual std::pair< Wt::WWidget*, Wt::WWidget* > initialize()
	{
		// Temp ?
		//set_agent_controllers(new rtp_sat::interactive_agent< dim >());

		m_widget = new ship_widget();
		m_widget->set_drawer(m_sys->get_drawer());
		m_widget->thruster_activated().connect(this, &ship_coordinator::on_widget_thruster_activated);

		// TODO: m_history_widget = new history_widget_t();

		return std::pair< Wt::WWidget*, Wt::WWidget* >(m_widget, nullptr);// m_history_widget);
	}

	void set_agent_controllers(agent_controller_t* a)
	{
		assert(a);
		m_sys->clear_agents();
		boost::optional< agent_id_t > id1 = m_sys->register_agent(a);
		assert(id1);
		m_controllers[*id1] = controller_ptr_t(a);
	}

	virtual void restart()
	{
		rgen_t rgen;
		rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
		
		m_sys->set_state(m_sys->generate_initial_state(rgen));

		m_widget->enable_interaction(true);	// TODO:
		// TODO: m_history_widget->reset();
		
		update_widgets();
		Wt::WTimer::singleShot(FrameDuration, this, &ship_coordinator::on_update);
	}

	void update_widgets()
	{
		m_widget->update();
		// TODO: m_history_widget->update();
	}

private:
	// TODO: This should just be on_widget_input(...), and the widget should be generic across all systems
	// (just define some basic input format, and the interactive agents define their own mapping)
	void on_widget_thruster_activated(size_t idx, bool activated)
	{
		controller_ptr_t controller = m_controllers.begin()->second;	// TODO: For now assuming only one agent
		// TODO: This seems bit dodgy
		// Should probably have a generic interface for interactive agents (see also above comment)
		boost::static_pointer_cast< rtp_sat::interactive_agent< dim > >(controller)->register_input(idx, activated);
	}

	void on_update()
	{
		bool result = m_sys->update(nullptr);

		update_widgets();

		if(result)
		{
			Wt::WTimer::singleShot(FrameDuration, this, &ship_coordinator::on_update);
		}
	}

private:
	system_t* m_sys;
	widget_t* m_widget;
	//history_widget_t* m_history_widget;

	std::map< agent_id_t, controller_ptr_t > m_controllers;
};


#endif

