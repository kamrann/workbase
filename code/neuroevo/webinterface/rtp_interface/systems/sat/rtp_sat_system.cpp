// rtp_sat_system.cpp

#include "rtp_sat_system.h"
#include "rtp_sat_scenario.h"
#include "rtp_sat_system_drawer.h"


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

	template < WorldDimensionality dim >
	rtp_named_param_list sat_system< dim >::params()
	{
		rtp_named_param_list p;
		//	p.push_back(rtp_named_param(new rtp_dimensionality_param_type(), "Dimensions"));
		p.push_back(rtp_named_param(new sat_scenario< dim >::param_type(), "Scenario"));
		return p;
	}

	template < WorldDimensionality dim >
	sat_system< dim >::sat_system(rtp_param param)
	{
		m_scenario = sat_scenario< dim >::create_instance(param);
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
			m_agent = agent;
			return boost::optional< agent_id_t >((agent_id_t)0);
		}
	}

	template < WorldDimensionality dim >
	void sat_system< dim >::generate_initial_state(rgen_t& rgen)
	{
		m_state = m_scenario->generate_initial_state(rgen);
	}

	template < WorldDimensionality dim >
	bool sat_system< dim >::update()
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

		return !m_scenario->is_complete(m_state);
	}

	template < WorldDimensionality dim >
	i_system_drawer* sat_system< dim >::get_drawer() const
	{
		return new sat_system_drawer< dim >(*this);
	}


	template class sat_system< WorldDimensionality::dim2D >;

}

