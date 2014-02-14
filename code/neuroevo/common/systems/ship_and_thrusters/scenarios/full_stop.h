// full_stop.h

#ifndef __SAT_FULL_STOP_H
#define __SAT_FULL_STOP_H

#include "../ship_and_thrusters_system.h"
#include "thrusters/ship_state.h"

#include <Eigen/StdVector>

/*
Agent is given initial linear and angular velocities, and is free to control thrusters either until both speeds are
sufficiently close to zero, or some predefined number of timesteps have elapsed.
*/

template <
	WorldDimensionality Dim_
>
class full_stop
{
public:
	static WorldDimensionality const Dim = Dim_;

	typedef ship_and_thrusters_system< Dim > system_t;
	typedef typename system_t::dim_traits_t dim_traits_t;

	/*
	Structure for state of a single agent.
	*/
	typedef ship_state< Dim > agent_state;

	/*
	This structure represents the state of the problem domain/environment, excluding that of the contained agents.
	*/
	struct envt_state {};

	/*
	This is the state known to a given agent, and represents all the information available to that agent to make decisions.
	*/
	struct agent_sensor_state
	{
		typename dim_traits_t::linear_velocity_t lin_vel;
		typename dim_traits_t::angular_velocity_t ang_vel;

		agent_sensor_state(agent_state const& a)
		{
			lin_vel = a.lin_velocity;
			ang_vel = a.ang_velocity;
		}

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	};

	/*
	Overall state, environment plus all agents.
	*/
	struct state: envt_state
	{
		std::vector< agent_state, Eigen::aligned_allocator< agent_state > > agents;

		inline agent_sensor_state get_sensor_state(size_t iagent) const
		{
			return agent_sensor_state(agents[iagent]);
		}
	};


	template < typename RGen >
	static inline void init_state(size_t num_agents, state& st, RGen& rgen)
	{
		st = state();
		st.agents.resize(num_agents);	// TODO: this should be tied to register_agent()

		for(size_t i = 0; i < num_agents; ++i)
		{
			st.agents[i].lin_velocity = random_val< typename dim_traits_t::velocity_t >(rgen);
			st.agents[i].ang_velocity = random_val< typename dim_traits_t::angular_velocity_t >(rgen);
		}
	}

	// TODO: should be part of system?
	struct system_agent_config
	{
		size_t num_registered;

		system_agent_config(): num_registered(0)
		{}
	};

	typedef typename size_t agent_id_t;

	static boost::optional< agent_id_t > register_agent(system_agent_config& sa_cfg)
	{
		return sa_cfg.num_registered++;
	}

	static inline bool pending_decision(agent_id_t a_id, state const& st)
	{
		return true;
	}

	struct system_update_params
	{
		std::vector< typename system_t::solution_result > decisions;
	};

	static void register_solution_decision(typename system_t::solution_result const& res, system_update_params& sup)
	{
		sup.decisions.push_back(res);
	}
	
	static bool update(state& st, system_update_params const& sup)
	{
		double const timestep = 1.0;	// TODO: ?

		assert(sup.decisions.size() == st.agents.size());

		for(size_t i = 0; i < st.agents.size(); ++i)
		{
			typename system_t::solution_result const& decision = sup.decisions[i];
			agent_state& a_state = st.agents[i];
			
			a_state.lin_velocity += decision.force * system_t::ThrusterPower * timestep;
			a_state.ang_velocity += decision.torque * system_t::ThrusterPower * timestep;

			a_state.position += a_state.lin_velocity * timestep;
			a_state.orientation = dim_traits_t::apply_orientation_delta(
				a_state.orientation, a_state.ang_velocity * timestep);
		}

		return true;	// TODO:
	}
};


#endif


