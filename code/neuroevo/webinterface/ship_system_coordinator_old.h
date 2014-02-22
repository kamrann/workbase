// ship_system_coordinator.h

#ifndef __SHIP_SYSTEM_COORDINATOR_H
#define __SHIP_SYSTEM_COORDINATOR_H

#include "system_coordinator.h"
#include "wt_system_widgets/ship_widget.h"
#include "wt_system_widgets/ship_history_widget.h"

//
#include "systems/ship_and_thrusters/genetic_mappings/fixednn_sln.h"
//

#include <Wt/WTimer>

#include <chrono>


template <
	typename Scenario_
>
class ship_coordinator: public system_coordinator
{
public:
	typedef Scenario_								scenario_t;
	typedef typename scenario_t::system_t			system_t;
	typedef typename system_t::solution_result		decision_t;

	static WorldDimensionality const Dim = system_t::Dim;

	enum {
		FrameDuration = 50,
	};

	typedef typename scenario_t::state				state_t;
	typedef typename scenario_t::agent_id_t			agent_id_t;
	
	typedef ship_widget< Dim >						widget_t;
	typedef ship_history_widget< Dim >				history_widget_t;

	class agent_controller: public agent_controller_base
	{
	public:
		virtual decision_t get_decision(state_t const& st) = 0;
	};

	typedef agent_controller						agent_controller_t;

	class ui_agent_controller: public agent_controller
	{
	public:
		virtual bool is_synchronous() const
		{
			return true;
		}

		virtual decision_t get_decision(state_t const&)
		{
			return ta;
		}

		ui_agent_controller()
		{
			// TODO: size synced with ship cfg
			ta.resize(thruster_presets::square_minimal().num_thrusters(), false);
		}

		void activate(size_t idx, bool activate)
		{
			if(idx < ta.size())
			{
				ta[idx] = activate;
			}
		}

	private:
		thruster_base::thruster_activation ta;
	};

	template < typename Solution >
	class nn_controller: public agent_controller
	{
	public:
		//typedef sat::angular_full_stop_fixednn_solution< system_t, typename scenario_t::agent_sensor_state > nn_agent_t;
		typedef Solution nn_agent_t;

	public:
		nn_controller(nn_agent_t const& a): m_agent(a)
		{}

		virtual bool is_synchronous() const
		{
			return true;
		}

		virtual decision_t get_decision(state_t const& st)	// TODO: st should be const&, but currently can't be due to embedded rgen
		{
			return m_agent(st.get_sensor_state(0));	// TODO: 0
		}

	private:
		nn_agent_t m_agent;
	};

public:
	virtual std::pair< Wt::WWidget*, Wt::WWidget* > initialize()
	{
		set_agent_controllers();

		m_widget = new widget_t();
		m_widget->thruster_activated().connect(this, &ship_coordinator::on_widget_thruster_activated);

		m_history_widget = new history_widget_t();

		return std::pair< Wt::WWidget*, Wt::WWidget* >(m_widget, m_history_widget);
	}

	void set_agent_controllers(agent_controller* c = nullptr)
	{
		scenario_t::system_agent_config sa_cfg;
		boost::optional< scenario_t::agent_id_t > id1 = scenario_t::register_agent(sa_cfg);
		assert(id1);
		m_controllers[*id1] = controller_ptr_t(c != nullptr ? c : new ui_agent_controller);
	}

	virtual void restart()
	{
		boost::random::mt19937 rgen;
		rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
		scenario_t::init_state(1, m_st, rgen);

		m_widget->enable_interaction(true);	// TODO:
		m_history_widget->reset();
		update_widgets();
	}

	void update_widgets()
	{
		sat_system< Dim >::scen_state< scenario_t >* st_interface = new sat_system< Dim >::scen_state< scenario_t >(m_st);
		m_widget->update_from_system_state(st_interface);
		m_history_widget->update_from_system_state(st_interface);

		Wt::WTimer::singleShot(FrameDuration, this, &ship_coordinator::on_agent_ready);
	}

private:
	void on_widget_thruster_activated(size_t idx, bool activated)
	{
		controller_ptr_t controller = m_controllers.begin()->second;	// TODO: For now assuming only one agent
		boost::static_pointer_cast< ui_agent_controller >(controller)->activate(idx, activated);
	}

	void on_agent_ready()
	{
		scenario_t::system_update_params sup;
		sup.timestep = FrameDuration / 1000.0;
		for(auto const& c_elem : m_controllers)
		{
			controller_ptr_t controller = c_elem.second;
			decision_t decision = controller->get_decision(m_st);
			scenario_t::register_solution_decision(decision, sup);
		}

		bool result = scenario_t::update(m_st, sup);
		if(result)
		{
			update_widgets();
		}
	}

private:
	state_t m_st;
	widget_t* m_widget;
	history_widget_t* m_history_widget;

	typedef boost::shared_ptr< agent_controller_t > controller_ptr_t;
	std::map< agent_id_t, controller_ptr_t > m_controllers;
};


#endif

