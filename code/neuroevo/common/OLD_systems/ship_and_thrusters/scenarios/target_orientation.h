// target_orientation.h

#ifndef __SAT_TARGET_ORIENTATION_H
#define __SAT_TARGET_ORIENTATION_H

#include "../ship_and_thrusters_system.h"
#include "thrusters/ship_state.h"
// TEMP
#include "thrusters/thruster_presets.h"
//

#include <Eigen/StdVector>

/*
Agent is given initial linear and angular velocities, and is free to control thrusters either until both speeds are
sufficiently close to zero, or some predefined number of timesteps have elapsed.
*/

template <
	WorldDimensionality Dim_
>
class target_orientation
{
public:
	static WorldDimensionality const Dim = Dim_;

	typedef ship_and_thrusters_system< Dim > system_t;
	typedef typename system_t::dim_traits_t dim_traits_t;

	/*
	Structure for state of a single agent.
	*/
	struct agent_state: public ship_state< Dim >
	{
		// TODO: Separate core state from superfluous state - for example, thruster heat, which is used only for display purposes
		thruster_system< Dim > thruster_sys;

		agent_state(): thruster_sys(nullptr)
		{}
	};

	/*
	This structure represents the state of the problem domain/environment, excluding that of the contained agents.
	*/
	struct envt_state
	{
		double time;

		envt_state(): time(0.0)
		{}
	};

	/*
	This is the state known to a given agent, and represents all the information available to that agent to make decisions.
	*/
	struct agent_sensor_state
	{
		typename dim_traits_t::orientation_t orient;
		typename dim_traits_t::angular_velocity_t ang_vel;

		agent_sensor_state(agent_state const& a)
		{
			orient = a.orientation;
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
	static inline void init_state(size_t num_agents, state& st, RGen& rgen, boost::optional< std::pair< size_t, size_t > > trial_context = boost::none)
	{
		st = state();
		st.agents.resize(num_agents);	// TODO: this should be tied to register_agent()

		for(size_t i = 0; i < num_agents; ++i)
		{
			// TODO:
			st.agents[i].thruster_cfg = boost::shared_ptr< thruster_config< Dim > >(new thruster_config< Dim >(thruster_presets::square_minimal()));
			st.agents[i].thruster_sys.cfg = st.agents[i].thruster_cfg;
			st.agents[i].thruster_sys.sync_to_cfg();

			// TODO: Random orientation routine for arbitrary dimensions
			st.agents[i].orientation = boost::random::uniform_01<>()(rgen) * 2 * boost::math::double_constants::pi;

			double const InitialAngularSpeed = 0.5;
			if(Dim == WorldDimensionality::dim2D && trial_context)
			{
				// Ensure half of trials have -ve initial rotation and half have +ve
				st.agents[i].ang_velocity = (trial_context->first % 2 == 0) ? InitialAngularSpeed : -InitialAngularSpeed;
			}
			else
			{
				st.agents[i].ang_velocity = random_dir_val< typename dim_traits_t::angular_velocity_t >(rgen) * InitialAngularSpeed;
			}
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
		double timestep;
		std::vector< typename system_t::solution_result > decisions;

		system_update_params(): timestep(0.05)	// Default to 20 updates per second of internal simulation system time
		{}
	};

	static void register_solution_decision(typename system_t::solution_result const& res, system_update_params& sup)
	{
		sup.decisions.push_back(res);
	}
	
	static bool update(state& st, system_update_params const& sup)
	{
		double const timestep = sup.timestep;// 1.0;	// TODO: ?
		double const thruster_force = system_t::ThrusterPower / 1000.0;

		assert(sup.decisions.size() == st.agents.size());

		for(size_t i = 0; i < st.agents.size(); ++i)
		{
			agent_state& a_state = st.agents[i];

			typename system_t::solution_result const& decision = sup.decisions[i];
			typename dim_traits_t::force_t force;
			typename dim_traits_t::torque_t torque;
			std::tie(force, torque) = a_state.thruster_cfg->calc_resultants(decision, a_state.c_of_mass);
			force = dim_traits_t::transform_dir(force, a_state.orientation);
			torque = dim_traits_t::transform_dir(torque, a_state.orientation);

			a_state.lin_velocity += force * thruster_force * timestep;
			a_state.ang_velocity += torque * thruster_force * timestep;

			a_state.position += a_state.lin_velocity * timestep;
			a_state.orientation = dim_traits_t::apply_orientation_delta(
				a_state.orientation, a_state.ang_velocity * timestep);

			for(size_t j = 0; j < a_state.thruster_sys.cfg->num_thrusters(); ++j)
			{
				if(decision[j])
				{
					a_state.thruster_sys[j].t.engage();
				}
				else
				{
					a_state.thruster_sys[j].t.cool_down(timestep);
				}
			}
		}

		st.time += timestep;

		return st.time < 10.0;	// TODO:
	}
};


#endif


