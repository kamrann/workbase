// ship_system_coordinator.h

#ifndef __SHIP_SYSTEM_COORDINATOR_H
#define __SHIP_SYSTEM_COORDINATOR_H

#include "system_coordinator.h"
#include "wt_system_widgets/ship_widget.h"

#include <Wt/WTimer>


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
		FrameDuration = 100,
	};

	typedef typename scenario_t::state				state_t;
	typedef typename scenario_t::agent_id_t			agent_id_t;
	
	typedef ship_widget< Dim >						widget_t;

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
			decision_t dec;
			// TODO: Use key state to generate thruster activations
			return dec;
		}
	};

public:
	virtual Wt::WWidget* initialize()
	{
		set_agent_controllers();

		m_widget = new widget_t();

		return m_widget;
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
		boost::random::mt19937 rgen_tmp;
		scenario_t::init_state(1, m_st, rgen_tmp);

		m_widget->enable_interaction(true);	// TODO:
		update_widget();
	}

	void update_widget()
	{
		m_widget->update_from_system_state(m_st);

		Wt::WTimer::singleShot(FrameDuration, this, &ship_coordinator::on_agent_ready);
	}

private:
	void on_agent_ready()
	{
		scenario_t::system_update_params sup;
		for(auto const& c_elem : m_controllers)
		{
			controller_ptr_t controller = c_elem.second;
			decision_t decision = controller->get_decision(m_st);
			scenario_t::register_solution_decision(decision, sup);
		}

		bool result = scenario_t::update(m_st, sup);
		if(result)
		{
			update_widget();
		}
	}

private:
	state_t m_st;
	widget_t* m_widget;

	typedef boost::shared_ptr< agent_controller_t > controller_ptr_t;
	std::map< agent_id_t, controller_ptr_t > m_controllers;
};


#endif

