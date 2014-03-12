// rtp_sat_system.cpp

#include "rtp_sat_system.h"
#include "rtp_sat_scenario.h"
#include "rtp_sat_system_drawer.h"
#include "rtp_sat_observers.h"
#include "scenarios/full_stop/agents.h"	// TODO: shouldn't be including scenario type specific stuff in this file...
#include "../rtp_interactive_input.h"
#include "../../params/paramlist_par.h"
#include "../../rtp_properties.h"

#include "rtp_sat_objectives.h"
#include "systems/ship_and_thrusters/objectives/minimize_linear_speed.h"
#include "systems/ship_and_thrusters/objectives/minimize_angular_speed.h"
#include "systems/ship_and_thrusters/objectives/minimize_kinetic_energy.h"
#include "systems/ship_and_thrusters/objectives/minimize_fuel_usage.h"

#include "thrusters/thruster_presets.h"

#include <Wt/WComboBox>


namespace rtp_sat {

	// TODO: May want to eventually separate i_agent into i_agent & i_agent_controller
	// (So we specify agent properties (such as ship configuration), and then subsequently specify a compatible controller (with its own properties)

	template < WorldDimensionality dim >
	std::string const sat_system< dim >::StateValueNames[] = {
		"Time",
		"PosX",
		"PosY",
		// Z ?
		"Angle",
		"VelX",
		"VelY",
		"Speed",
		"AngularSpeed",
		"KE",
	};

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
		"Reduce Speed",
		"Minimize Avg Speed",
		"Reduce Angular Speed",
		"Minimize Avg Angular Speed",
		"Reduce Kinetic Energy",
		"Minimize Avg Kinetic Energy",
//		"Minimize Fuel Usage",
	};

	template < WorldDimensionality dim >
	sat_system< dim >::ship_config::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	template < WorldDimensionality dim >
	thruster_config< dim > sat_system< dim >::ship_config::create_instance(Type type, rtp_param param)
	{
		thruster_config< dim > cfg;
		switch(type)
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
	sat_system< dim >::i_sat_agent::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	template < WorldDimensionality dim >
	rtp_param_type* sat_system< dim >::i_sat_agent::params(typename sat_system< dim >::i_sat_agent::Type type)//typename ship_config::Type cfg_type)
	{
		switch(type)
		{
			case i_sat_agent::Passive:
			// TODO:
			return new rtp_staticparamlist_param_type(rtp_named_param_list());

			case i_sat_agent::Interactive:
			// TODO:
			return new rtp_staticparamlist_param_type(rtp_named_param_list());
			
			default:
			return nullptr;
		}
	}

	template < WorldDimensionality dim >
	typename sat_system< dim >::i_sat_agent* sat_system< dim >::i_sat_agent::create_instance(Type type, rtp_param param)
	{
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
	sat_system< dim >::evolvable_agent::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	template < WorldDimensionality dim >
	rtp_param_type* sat_system< dim >::evolvable_agent::params(Type ea_type)
	{
		switch(ea_type)
		{
		case MLP:
			return new mlp_agent< dim >::param_type();

		default:
			assert(false);
			return nullptr;
		}
	}

	template < WorldDimensionality dim >
	std::tuple< i_genome_mapping*, i_agent_factory* > sat_system< dim >::evolvable_agent::create_instance(Type type, rtp_param param, thruster_config< dim > const& cfg)
	{
		//auto param_list = boost::any_cast<rtp_param_list>(param);

		switch(type)
		{
		case MLP:
			return mlp_agent< dim >::create_instance_evolvable(param/*param_list*/, cfg);
			break;

		default:
			assert(false);
			return std::tuple< i_genome_mapping*, i_agent_factory* >(nullptr, nullptr);
		}
	}

	template < WorldDimensionality dim >
	sat_system< dim >::agent_objective::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	template < WorldDimensionality dim >
	rtp_param_type* sat_system< dim >::agent_objective::params(Type type)
	{
		switch(type)
		{
			// TODO: 
			default:
			return new rtp_staticparamlist_param_type(rtp_named_param_list());
		}
	}

	template < WorldDimensionality dim >
	i_observer* sat_system< dim >::agent_objective::create_instance(Type type, rtp_param param)
	{
		switch(type)
		{
			case ReduceSpeed:			return new rtp_sat::wrapped_objective_fn< dim, reduce_lin_speed_obj_fn >();
			case MinAvgSpeed:			return new rtp_sat::wrapped_objective_fn< dim, min_avg_lin_speed_obj_fn >();
			case ReduceAngularSpeed:	return new rtp_sat::wrapped_objective_fn< dim, reduce_ang_speed_obj_fn >();
			case MinAvgAngularSpeed:	return new rtp_sat::wrapped_objective_fn< dim, min_avg_ang_speed_obj_fn >();
			case ReduceKinetic:			return new rtp_sat::wrapped_objective_fn< dim, reduce_kinetic_obj_fn >();
			case MinAvgKinetic:			return new rtp_sat::wrapped_objective_fn< dim, min_avg_kinetic_obj_fn >();
//			case MinFuel:				return new rtp_sat::wrapped_objective_fn< dim, min_fuel_obj_fn >();

			default:
			assert(false);
			return nullptr;
		}
	}


	template < WorldDimensionality dim >
	size_t sat_system< dim >::param_type::provide_num_child_params(rtp_param_manager* mgr) const
	{
		return m_evolvable ? 6 : 4;
	}

	template < WorldDimensionality dim >
	rtp_named_param sat_system< dim >::param_type::provide_child_param(size_t index, rtp_param_manager* mgr) const
	{
		switch(index)
		{
			case 0:
			return rtp_named_param(sat_scenario< dim >::params());

			case 1:
			return rtp_named_param(new ship_config::enum_param_type(), "Ship Config");

			case 2:
			return m_evolvable ? rtp_named_param(new evolvable_agent::enum_param_type(), "Controller") : rtp_named_param(new i_sat_agent::enum_param_type(), "Controller");

			case 3:
			if(m_evolvable)
			{
				evolvable_agent::Type ea = boost::any_cast<evolvable_agent::Type>(mgr->retrieve_param("Controller"));
				return rtp_named_param(evolvable_agent::params(ea));
			}
			else
			{
				i_sat_agent::Type a = boost::any_cast<i_sat_agent::Type>(mgr->retrieve_param("Controller"));
				return rtp_named_param(i_sat_agent::params(a));
			}

			case 4:
			return rtp_named_param(new agent_objective::enum_param_type(), "Objective");

			case 5:
			{
				agent_objective::Type ao = boost::any_cast<agent_objective::Type>(mgr->retrieve_param("Objective"));
				return rtp_named_param(agent_objective::params(ao));
			}

			default:
			return rtp_named_param();
		}
	}

	template < WorldDimensionality dim >
	rtp_param_type* sat_system< dim >::params(bool evolvable)
	{
/*		rtp_named_param_list p;
		//	p.push_back(rtp_named_param(new rtp_dimensionality_param_type(), "Dimensions"));

		Try implementing:
		sub_param_type - wraps an rtp_param_type, in create_widget indents its child, just passes child's param in get_param
		group_param_type - wraps a child rtp_param_type within a group box/panel, as above for get_param

		p.push_back(rtp_named_param(sat_scenario< dim >::params()));
		p.push_back(rtp_named_param(new ship_config::param_type(), "Ship Config"));
		p.push_back(rtp_named_param(evolvable ? (rtp_param_type*)evolvable_agent::params() : (rtp_param_type*)new i_sat_agent::enum_param_type(), "Controller"));

		return new rtp_staticparamlist_param_type(p);
		*/

		return new param_type(evolvable);
	}

	template < WorldDimensionality dim >
	std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* > sat_system< dim >::create_instance(rtp_param param, bool evolvable)
	{
		std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* > result;

		rtp_param_list param_list = boost::any_cast<rtp_param_list>(param);
		
		sat_scenario< dim >* scenario = sat_scenario< dim >::create_instance(param_list[0]);
		
		ship_config::Type cfg_type = boost::any_cast<ship_config::Type>(param_list[1]);
		thruster_config< dim > ship_cfg = ship_config::create_instance(cfg_type, rtp_param());

		std::get< 0 >(result) = new sat_system< dim >(scenario, ship_cfg);
		if(evolvable)
		{
			evolvable_agent::Type ea_type = boost::any_cast<evolvable_agent::Type>(param_list[2]);
			// TODO: Some easy way to access sub-tuple? (eg. result.firstN)
			std::tie(
				std::get< 1 >(result),
				std::get< 2 >(result)
				) = evolvable_agent::create_instance(ea_type, param_list[3], ship_cfg);

			agent_objective::Type obj_type = boost::any_cast<agent_objective::Type>(param_list[4]);
			std::get< 3 >(result) = agent_objective::create_instance(obj_type, param_list[5]);
		}
		else
		{
			i_sat_agent::Type a_type = boost::any_cast<i_sat_agent::Type>(param_list[2]);
			boost::shared_ptr< i_sat_agent > agent(i_sat_agent::create_instance(a_type, param_list[3]));
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
	boost::optional< agent_id_t > sat_system< dim >::register_agent(boost::shared_ptr< i_agent > agent)
	{
		// For now, allowing only a single agent in the system
		if(m_agent)
		{
			return boost::none;
		}
		else
		{
			m_agent = boost::static_pointer_cast< i_sat_agent >(agent);
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
	void sat_system< dim >::register_interactive_input(interactive_input const& input)
	{
		// TODO: Maybe store within create_instance a pointer to any interactive controller, or provide a method on i_agent
		// along lines of is_interactive()
		if(m_agent)
		{
			boost::dynamic_pointer_cast<i_interactive_agent>(*m_agent)->register_input(input);
		}
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
	i_observer::observations_t sat_system< dim >::record_observations(i_observer* obs) const
	{
		i_sat_observer< dim >* sat_obs = (i_sat_observer< dim >*)obs;
		return sat_obs->record_observations(m_td);
	}

	template < WorldDimensionality dim >
	void sat_system< dim >::set_state_prop(rtp_stored_property_values* pv, StateValue sv, boost::any v)
	{
		pv->set_value(StateValueNames[sv], v);
	}

	template < WorldDimensionality dim >
	boost::shared_ptr< i_properties const > sat_system< dim >::get_state_properties() const
	{
		rtp_stored_properties* props = new rtp_stored_properties();
		for(size_t i = 0; i < StateValue::Count; ++i)
		{
			props->add_property(StateValueNames[i]);
		}
		return boost::shared_ptr< i_properties const >(props);
	}

	template < WorldDimensionality dim >
	boost::shared_ptr< i_property_values const > sat_system< dim >::get_state_property_values() const
	{
		rtp_stored_property_values* vals = new rtp_stored_property_values();
		set_state_prop(vals, Time, m_state.time);
		set_state_prop(vals, PosX, m_state.ship.position[0]);
		set_state_prop(vals, PosY, m_state.ship.position[1]);
		set_state_prop(vals, Angle, m_state.ship.orientation);
		set_state_prop(vals, VelX, m_state.ship.lin_velocity[0]);
		set_state_prop(vals, VelY, m_state.ship.lin_velocity[1]);
		double speed = magnitude(m_state.ship.lin_velocity);
		set_state_prop(vals, Speed, speed);
		double ang_speed = m_state.ship.ang_velocity;	// TODO: For 3D should be magnitude()...
		set_state_prop(vals, AngularSpeed, ang_speed);
		double ke = 0.5 * (speed * speed + ang_speed * ang_speed);	// TODO: m and I
		set_state_prop(vals, KE, ke);
		return boost::shared_ptr< i_property_values const >(vals);
	}

	template < WorldDimensionality dim >
	i_system_drawer* sat_system< dim >::get_drawer() const
	{
		return new sat_system_drawer< dim >(*this);
	}


	template class sat_system< WorldDimensionality::dim2D >;
	//template class sat_system< WorldDimensionality::dim3D >;
}

