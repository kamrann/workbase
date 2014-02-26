// rtp_sat_system.cpp

#include "rtp_sat_system.h"
#include "rtp_sat_scenario.h"
#include "rtp_sat_system_drawer.h"
#include "rtp_sat_observers.h"
#include "scenarios/full_stop/agents.h"	// TODO: shouldn't be including scenario type specific stuff in this file...
#include "../../params/paramlist_par.h"

#include "rtp_sat_objectives.h"
#include "systems/ship_and_thrusters/objectives/minimize_linear_speed.h"
#include "systems/ship_and_thrusters/objectives/minimize_angular_speed.h"
#include "systems/ship_and_thrusters/objectives/minimize_kinetic_energy.h"
#include "systems/ship_and_thrusters/objectives/minimize_fuel_usage.h"

#include "thrusters/thruster_presets.h"

#include <Wt/WComboBox>


namespace rtp_sat {

/*	i_system* sat_system_base::create_instance(rtp_param param)
	{
		rtp_param_list& param_list = boost::any_cast<rtp_param_list&>(param);
		WorldDimensionality dim = boost::any_cast<WorldDimensionality>(param_list[0]);
		switch(dim)
		{
		case WorldDimensionality::dim2D:
			return new sat_system< WorldDimensionality::dim2D >(param_list[1]);

		case WorldDimensionality::dim3D:
			return new sat_system< WorldDimensionality::dim3D >(param_list[1]);

		default:
			return nullptr;
		}
	}
*/

	// TODO: May want to eventually separate i_agent into i_agent & i_agent_controller
	// (So we specify agent properties (such as ship configuration), and then subsequently specify a compatible controller (with its own properties)

	template < WorldDimensionality dim >
	std::string const sat_system< dim >::ship_config::Names[] = {
		"Square Minimal",
		"Square Complete",
	};

	template < WorldDimensionality dim >
	std::string const sat_system< dim >::i_sat_agent::Names[] = {
		"Passive",
		"Interactive",
	};

	template < WorldDimensionality dim >
	std::string const sat_system< dim >::evolvable_agent::Names[] = {
		"MLP",
	};

	template < WorldDimensionality dim >
	std::string const sat_system< dim >::agent_objective::Names[] = {
		"Minimize Speed",
		"Minimize Angular Speed",
		"Minimize Kinetic Energy",
//		"Minimize Fuel Usage",
	};

	template < WorldDimensionality dim >
	boost::any sat_system< dim >::ship_config::enum_param_type::default_value() const
	{
		return SquareMinimal;
	}

	template < WorldDimensionality dim >
	i_param_widget* sat_system< dim >::ship_config::enum_param_type::create_widget(rtp_param_manager* mgr) const
	{
		rtp_param_widget< Wt::WComboBox >* box = new rtp_param_widget< Wt::WComboBox >(this);

		for(size_t i = 0; i < Type::Count; ++i)
		{
			box->addItem(Names[i]);
			box->model()->setData(i, 0, (Type)i, Wt::UserRole);
		}

		return box;
	}

	template < WorldDimensionality dim >
	rtp_param sat_system< dim >::ship_config::enum_param_type::get_widget_param(i_param_widget const* w) const
	{
		Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
		Wt::WAbstractItemModel const* model = box->model();
		Type cfg = boost::any_cast< Type >(model->data(box->currentIndex(), 0, Wt::UserRole));
		return rtp_param(cfg);
	}

	template < WorldDimensionality dim >
	rtp_named_param sat_system< dim >::ship_config::param_type::provide_selection_param() const
	{
		return rtp_named_param(new sat_system< dim >::ship_config::enum_param_type(), "Ship Config");
	}

	template < WorldDimensionality dim >
	rtp_param_type* sat_system< dim >::ship_config::param_type::provide_nested_param(rtp_param_manager* mgr) const
	{
		Type cfg_type = boost::any_cast<Type>(mgr->retrieve_param("Ship Config"));
		rtp_named_param_list sub_params;// = m_evolvable ? evolvable_agent::params(cfg_type) : i_sat_agent::params(cfg_type);
		return new rtp_staticparamlist_param_type(sub_params);
	}

	template < WorldDimensionality dim >
	thruster_config< dim > sat_system< dim >::ship_config::create_instance(rtp_param param)
	{
		auto param_pr = boost::any_cast<std::pair< boost::any, boost::any >>(param);
		Type cfg_type = boost::any_cast<Type>(param_pr.first);

		thruster_config< dim > cfg;
		switch(cfg_type)
		{
		case SquareMinimal:
			cfg = thruster_presets::square_minimal();
			break;
		case SquareComplete:
			cfg = thruster_presets::square_complete();
			break;
		default:
			assert(false);
		}

		return cfg;
	}


	template < WorldDimensionality dim >
	boost::any sat_system< dim >::i_sat_agent::enum_param_type::default_value() const
	{
		return Passive;
	}

	template < WorldDimensionality dim >
	i_param_widget* sat_system< dim >::i_sat_agent::enum_param_type::create_widget(rtp_param_manager* mgr) const
	{
		rtp_param_widget< Wt::WComboBox >* box = new rtp_param_widget< Wt::WComboBox >(this);

		for(size_t i = 0; i < Type::Count; ++i)
		{
			box->addItem(Names[i]);
			box->model()->setData(i, 0, (Type)i, Wt::UserRole);
		}

		return box;
	}

	template < WorldDimensionality dim >
	rtp_param sat_system< dim >::i_sat_agent::enum_param_type::get_widget_param(i_param_widget const* w) const
	{
		Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
		Wt::WAbstractItemModel const* model = box->model();
		Type type = boost::any_cast< Type >(model->data(box->currentIndex(), 0, Wt::UserRole));
		return rtp_param(type);
	}
/*
	template < WorldDimensionality dim >
	rtp_named_param_list sat_system< dim >::i_sat_agent::params(typename ship_config::Type cfg_type)
	{
		rtp_named_param_list p;

		switch(cfg_type)
		{
			// TODO:
		default:
			{
				p.push_back(rtp_named_param(new enum_param_type(), "Controller"));
			}
			break;
		}
		return p;
	}
*/
	template < WorldDimensionality dim >
	typename sat_system< dim >::i_sat_agent* sat_system< dim >::i_sat_agent::create_instance(rtp_param param)
	{
//		auto param_pr = boost::any_cast<std::pair< rtp_param, rtp_param >>(param);
//		Type type = boost::any_cast<Type>(param_pr.first);
//		rtp_param_list sub = boost::any_cast<rtp_param_list>(param_pr.second);
		Type type = boost::any_cast<Type>(param);
		switch(type)
		{
		case Passive:
			return new passive_agent< dim >(/*sub*/);
		case Interactive:
			return new interactive_agent< dim >(/*sub*/);
		default:
			return nullptr;
		}
	}

	template < WorldDimensionality dim >
	boost::any sat_system< dim >::evolvable_agent::enum_param_type::default_value() const
	{
		return MLP;
	}

	template < WorldDimensionality dim >
	i_param_widget* sat_system< dim >::evolvable_agent::enum_param_type::create_widget(rtp_param_manager* mgr) const
	{
		rtp_param_widget< Wt::WComboBox >* box = new rtp_param_widget< Wt::WComboBox >(this);

		for(size_t i = 0; i < Type::Count; ++i)
		{
			box->addItem(Names[i]);
			box->model()->setData(i, 0, (Type)i, Wt::UserRole);
		}

		return box;
	}

	template < WorldDimensionality dim >
	rtp_param sat_system< dim >::evolvable_agent::enum_param_type::get_widget_param(i_param_widget const* w) const
	{
		Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
		Wt::WAbstractItemModel const* model = box->model();
		Type type = boost::any_cast< Type >(model->data(box->currentIndex(), 0, Wt::UserRole));
		return rtp_param(type);
	}

	template < WorldDimensionality dim >
	rtp_named_param sat_system< dim >::evolvable_agent::param_type::provide_selection_param() const
	{
		return rtp_named_param(new enum_param_type(), "Controller");
	}

	template < WorldDimensionality dim >
	rtp_param_type* sat_system< dim >::evolvable_agent::param_type::provide_nested_param(rtp_param_manager* mgr) const
	{
		Type ea_type = boost::any_cast<Type>(mgr->retrieve_param("Controller"));
		rtp_named_param_list sub_params = evolvable_agent::params(ea_type);
		return new rtp_staticparamlist_param_type(sub_params);
	}

	template < WorldDimensionality dim >
	rtp_named_param_list sat_system< dim >::evolvable_agent::params()
	{
		rtp_named_param_list p;
		p.push_back(rtp_named_param(new param_type(), "Evolvable Agent?"));
		p.push_back(rtp_named_param(new rtp_staticparamlist_param_type(agent_objective::params()), "Objective"));
		return p;
	}

	template < WorldDimensionality dim >
	rtp_named_param_list sat_system< dim >::evolvable_agent::params(Type ea_type)
	{
		rtp_named_param_list p;

		switch(ea_type)
		{
		case MLP:
			p.push_back(rtp_named_param(new mlp_agent< dim >::param_type()));
			break;

		default:
			assert(false);
		}

		return p;
	}

	template < WorldDimensionality dim >
	std::tuple< i_genome_mapping*, i_agent_factory*, i_observer* > sat_system< dim >::evolvable_agent::create_instance(rtp_param param, thruster_config< dim > const& cfg)
	{
		auto param_list = boost::any_cast<rtp_param_list>(param);
		auto param_pr = boost::any_cast<std::pair< rtp_param, rtp_param >>(param_list[0]);

		std::pair< i_genome_mapping*, i_agent_factory* > agent_spec;
		Type type = boost::any_cast<Type>(param_pr.first);
		switch(type)
		{
		case MLP:
			agent_spec = mlp_agent< dim >::create_instance_evolvable(param_pr.second, cfg);
			break;

		default:
			assert(false);
		}

		std::tuple< i_genome_mapping*, i_agent_factory*, i_observer* > result;
		std::get< 0 >(result) = agent_spec.first;
		std::get< 1 >(result) = agent_spec.second;

		std::get< 2 >(result) = agent_objective::create_instance(param_list[1]);
		return result;
	}

	template < WorldDimensionality dim >
	boost::any sat_system< dim >::agent_objective::enum_param_type::default_value() const
	{
		return MinSpeed;
	}

	template < WorldDimensionality dim >
	i_param_widget* sat_system< dim >::agent_objective::enum_param_type::create_widget(rtp_param_manager* mgr) const
	{
		rtp_param_widget< Wt::WComboBox >* box = new rtp_param_widget< Wt::WComboBox >(this);

		for(size_t i = 0; i < Type::Count; ++i)
		{
			box->addItem(Names[i]);
			box->model()->setData(i, 0, (Type)i, Wt::UserRole);
		}

		return box;
	}

	template < WorldDimensionality dim >
	rtp_param sat_system< dim >::agent_objective::enum_param_type::get_widget_param(i_param_widget const* w) const
	{
		Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
		Wt::WAbstractItemModel const* model = box->model();
		Type type = boost::any_cast< Type >(model->data(box->currentIndex(), 0, Wt::UserRole));
		return rtp_param(type);
	}

	template < WorldDimensionality dim >
	rtp_named_param_list sat_system< dim >::agent_objective::params()
	{
		rtp_named_param_list p;
		p.push_back(rtp_named_param(new enum_param_type(), "Objective?"));
		return p;
	}

	template < WorldDimensionality dim >
	i_observer* sat_system< dim >::agent_objective::create_instance(rtp_param param)
	{
		auto param_list = boost::any_cast<rtp_param_list>(param);
		Type type = boost::any_cast<Type>(param_list[0]);
		switch(type)
		{
			case MinSpeed:			return new rtp_sat::wrapped_objective_fn< dim, min_lin_speed_obj_fn >();
			case MinAngularSpeed:	return new rtp_sat::wrapped_objective_fn< dim, min_ang_speed_obj_fn >();
			case MinKinetic:		return new rtp_sat::wrapped_objective_fn< dim, min_kinetic_obj_fn >();
//			case MinFuel:			return new rtp_sat::wrapped_objective_fn< dim, min_fuel_obj_fn >();

			default:
			assert(false);
			return nullptr;
		}
	}


	template < WorldDimensionality dim >
	rtp_named_param_list sat_system< dim >::params(bool evolvable)
	{
		rtp_named_param_list p;
		//	p.push_back(rtp_named_param(new rtp_dimensionality_param_type(), "Dimensions"));
		p.push_back(rtp_named_param(//new sat_scenario< dim >::param_type(), "Scenario"));
			new rtp_staticparamlist_param_type(sat_scenario< dim >::params()), "Scenario"));
		p.push_back(rtp_named_param(new ship_config::param_type(), "Ship Config"));
		p.push_back(rtp_named_param(evolvable ? (rtp_param_type*)new rtp_staticparamlist_param_type(evolvable_agent::params())/*param_type()*/: (rtp_param_type*)new i_sat_agent::enum_param_type(), "Controller"));
		return p;
	}

	template < WorldDimensionality dim >
	std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* > sat_system< dim >::create_instance(rtp_param param, bool evolvable)
	{
		std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* > result;

		rtp_param_list param_list = boost::any_cast<rtp_param_list>(param);
		sat_scenario< dim >* scenario = sat_scenario< dim >::create_instance(param_list[0]);
		thruster_config< dim > ship_cfg = ship_config::create_instance(param_list[1]);

		std::get< 0 >(result) = new sat_system< dim >(scenario, ship_cfg);
		if(evolvable)
		{
			// TODO: Some easy way to access sub-tuple? (eg. result.firstN)
			std::tie(
				std::get< 1 >(result),
				std::get< 2 >(result),
				std::get< 3 >(result)
				) = evolvable_agent::create_instance(param_list[2], ship_cfg);
		}
		else
		{
			i_sat_agent* agent = i_sat_agent::create_instance(param_list[2]);
			std::get< 0 >(result)->register_agent(agent);
		}
		return result;
	}

	template < WorldDimensionality dim >
	sat_system< dim >::sat_system(sat_scenario< dim >* scenario, thruster_config< dim > const& t_cfg)
	{
		m_scenario = scenario;
		m_ship_cfg = t_cfg;
	}

	template < WorldDimensionality dim >
	void sat_system< dim >::clear_agents()
	{
		m_agent = boost::none;
	}

	template < WorldDimensionality dim >
	boost::optional< agent_id_t > sat_system< dim >::register_agent(i_agent* agent)
	{
		// For now, allowing only a single agent in the system
		if(m_agent)
		{
			return boost::none;
		}
		else
		{
			m_agent = (i_sat_agent*)agent;
			return boost::optional< agent_id_t >((agent_id_t)0);
		}
	}

	template < WorldDimensionality dim >
	boost::any sat_system< dim >::generate_initial_state(rgen_t& rgen) const
	{
		// TODO: Currently working on assumption that this does not depend on which agents are registered, so that can be called
		// once before start of trial, and used for any subsequently registered agent

		state st = m_scenario->generate_initial_state(rgen);

		st.ship.thruster_cfg = boost::shared_ptr< thruster_config< dim > >(new thruster_config< dim >(m_ship_cfg));
		st.thrusters.cfg = st.ship.thruster_cfg;
		st.thrusters.sync_to_cfg();

		return st;
	}

	template < WorldDimensionality dim >
	void sat_system< dim >::set_state(boost::any const& st)
	{
		m_state = boost::any_cast<state const&>(st);

		// TODO: If we want to use this method to set a general state, this shouldn't be here...
		m_td = trial_data();
		m_td.initial_st = m_state;
	}

	template < WorldDimensionality dim >
	bool sat_system< dim >::update(i_observer* obs)
	{
		double const timestep = 0.05;		// TODO: System parameter
		double const thruster_force = 1.0;	// TODO: Parameter

		/*
		This should generalise to something like:

		sys_update_data sud
		for each agent a:							// a is agent of unknown type
			register_agent_actions(a(state), sud)	// This uses virtual mechanism to determine what effect agent a's actions will have on system

		process_system_updates(sud)					// Update system taking into account all agents' actions simultaneously

		This way, we can have different types of agents, which can affect the system in different ways.
		We no longer have a single decision type, but rather let agent type specific code register whatever effects it needs on the system.
		*/

		decision const ta = (*m_agent)->make_decision(m_state, m_scenario->get_scenario_data());
		
		typename dim_traits_t::force_t force;
		typename dim_traits_t::torque_t torque;
		std::tie(force, torque) = m_state.ship.thruster_cfg->calc_resultants(ta, m_state.ship.c_of_mass);
		force = dim_traits_t::transform_dir(force, m_state.ship.orientation);
		torque = dim_traits_t::transform_dir(torque, m_state.ship.orientation);

		m_state.ship.lin_velocity += force * thruster_force * timestep;
		m_state.ship.ang_velocity += torque * thruster_force * timestep;

		m_state.ship.position += m_state.ship.lin_velocity * timestep;
		m_state.ship.orientation = dim_traits_t::apply_orientation_delta(
			m_state.ship.orientation, m_state.ship.ang_velocity * timestep);
		
		for(size_t j = 0; j < m_state.thrusters.cfg->num_thrusters(); ++j)
		{
			if(ta[j])
			{
				m_state.thrusters[j].t.engage();
			}
			else
			{
				m_state.thrusters[j].t.cool_down(timestep);
			}
		}

		m_state.time += timestep;

		if(obs)
		{
			i_sat_observer< dim >* sat_obs = (i_sat_observer< dim >*)obs;
			sat_obs->update(m_state);
		}

		m_td.final_st = m_state;

		return !m_scenario->is_complete(m_state);
	}

	template < WorldDimensionality dim >
	boost::any sat_system< dim >::record_observations(i_observer* obs) const
	{
		i_sat_observer< dim >* sat_obs = (i_sat_observer< dim >*)obs;
		return sat_obs->record_observations(m_td);
	}

	template < WorldDimensionality dim >
	i_system_drawer* sat_system< dim >::get_drawer() const
	{
		return new sat_system_drawer< dim >(*this);
	}


	template class sat_system< WorldDimensionality::dim2D >;
	//template class sat_system< WorldDimensionality::dim3D >;
}

