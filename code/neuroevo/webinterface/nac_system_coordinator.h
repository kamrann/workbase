// nac_system_coordinator.h

#ifndef __NAC_SYSTEM_COORDINATOR_H
#define __NAC_SYSTEM_COORDINATOR_H

#include "system_coordinator.h"
#include "wt_system_widgets/nac_widget.h"

//
#include "systems/noughts_and_crosses/genetic_mappings/fixed_neural_net.h"
//

#include <Wt/WTimer>


template <
	typename Scenario_
>
class nac_coordinator: public system_coordinator
{
public:
	typedef Scenario_								scenario_t;
	typedef typename scenario_t::system_t			system_t;
	typedef typename system_t::solution_result		decision_t;

	enum {
		Dimensions = system_t::Dimensions,
		Size = system_t::Size,
		AutoResponseTime = 2000,
	};

	typedef typename system_t::board_state			board_t;
	typedef typename board_t::coord_t				board_coord_t;

	typedef typename scenario_t::state				state_t;
	typedef typename scenario_t::agent_id_t			agent_id_t;
	
	typedef noughts_and_crosses_widget< Dimensions, Size >		widget_t;

	class agent_controller: public agent_controller_base
	{
	public:
		virtual decision_t get_decision(state_t& st) = 0;	// TODO: st should be const&, but currently can't be due to embedded rgen
	};

	typedef agent_controller						agent_controller_t;

	class ui_agent_controller: public agent_controller
	{
	public:
		virtual bool is_synchronous() const
		{
			return false;
		}

		virtual decision_t get_decision(state_t& st)	// TODO: st should be const&, but currently can't be due to embedded rgen
		{
			return m_decision;
		}

		virtual void set_decision(decision_t const& dec)
		{
			m_decision = dec;
		}

	private:
		decision_t m_decision;
	};

	class simple_controller: public agent_controller
	{
	public:
		virtual bool is_synchronous() const
		{
			return true;
		}

		virtual decision_t get_decision(state_t& st)	// TODO: st should be const&, but currently can't be due to embedded rgen
		{
			size_t flat = get_sensible_move< Dimensions, Size >(
				st,
				st.num_played_squares() % 2 == 0 ? typename system_t::Player::Crosses : typename system_t::Player::Noughts,
				st.rgen);
			return flat;// board_coord_t::from_flat_index(flat, Size);
		}
	};

	class nn_controller: public agent_controller
	{
	public:
		typedef fixed_neural_net< system_t, typename scenario_t::solution_input > genetic_mapping_t;
		typedef typename genetic_mapping_t::solution nn_agent_t;

	public:
		nn_controller(nn_agent_t const& a): m_agent(a)
		{}

		virtual bool is_synchronous() const
		{
			return true;
		}

		virtual decision_t get_decision(state_t& st)	// TODO: st should be const&, but currently can't be due to embedded rgen
		{
			return m_agent(st);
			//return board_coord_t::from_flat_index(flat, Size);
		}

	private:
		nn_agent_t m_agent;
	};

public:
	virtual Wt::WWidget* initialize()
	{
		set_agent_controllers();

		m_widget = new widget_t();
		m_widget->move_made().connect(this, &nac_coordinator::on_widget_move_made);

		return m_widget;
	}

	void set_agent_controllers(agent_controller* crosses = nullptr, agent_controller* noughts = nullptr)
	{
		scenario_t::system_agent_config sa_cfg;
		boost::optional< scenario_t::agent_id_t > id1 = scenario_t::register_agent(sa_cfg);
		assert(id1);
		m_controllers[*id1] = controller_ptr_t(crosses != nullptr ? crosses : new ui_agent_controller);
		boost::optional< scenario_t::agent_id_t > id2 = scenario_t::register_agent(sa_cfg);
		assert(id2);
		m_controllers[*id2] = controller_ptr_t(noughts != nullptr ? noughts : new ui_agent_controller);
	}

	virtual void restart()
	{
		boost::random::mt19937 rgen_tmp;
		scenario_t::init_state(2, m_st, rgen_tmp);

		//m_controllers.clear();
		//set_agent_controllers();

		update_widget(true);
	}

	void update_widget(bool still_playing)
	{
		boost::optional< agent_id_t > to_act = scenario_t::pending_agent(m_st);
		if(still_playing && to_act)
		{
			controller_ptr_t controller = m_controllers.at(*to_act);
			if(controller->is_synchronous())
			{
				Wt::WTimer::singleShot(AutoResponseTime, this, &nac_coordinator::on_agent_ready);
				m_widget->enable_interaction(false);
			}
			else
			{
				// TODO: Assuming asynchronous implies user interaction, but may want to allow asynchronous automated agents.
				// For example if their decision making process can be long.
				m_widget->enable_interaction(true);
			}
		}
		else
		{
			m_widget->enable_interaction(false);
		}

		m_widget->set_invalid_move(to_act && !still_playing);
		m_widget->update_from_system_state(m_st);
	}

private:
	void on_widget_move_made(board_coord_t cd)
	{
		boost::optional< agent_id_t > to_act = scenario_t::pending_agent(m_st);
		assert(to_act);
		controller_ptr_t controller = m_controllers.at(*to_act);
		boost::static_pointer_cast< ui_agent_controller >(controller)->set_decision(cd.to_flat_index(Size));

		on_agent_ready();
	}

	void on_agent_ready()
	{
		boost::optional< agent_id_t > to_act = scenario_t::pending_agent(m_st);
		assert(to_act);
		controller_ptr_t controller = m_controllers.at(*to_act);

		decision_t decision = controller->get_decision(m_st);
		scenario_t::system_update_params sup;
		scenario_t::register_solution_decision(decision, sup);
		bool result = scenario_t::update(m_st, sup);

		update_widget(result);
	}

private:
	state_t m_st;
	widget_t* m_widget;

	typedef boost::shared_ptr< agent_controller_t > controller_ptr_t;
	std::map< agent_id_t, controller_ptr_t > m_controllers;
};


#endif

