// angular_full_stop.h

#ifndef __ANGULAR_FULL_STOP_H
#define __ANGULAR_FULL_STOP_H

#include "ga/fitness.h"
#include "ga/composite_fitness_fn.h"

#include "problem_base.h"
#include "dimensionality.h"
#include "ship_state.h"

#include "Eigen/Dense"

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/tuple/tuple.hpp>

#include <vector>
#include <cmath>
#include <algorithm>

#if 1
#include <iostream>
#endif


//const double AcceptableAngularSpeed = 0.00001f;


class angular_full_stop
{
public:
	// TODO: Class should be templated on dimensionality
	static const WorldDimensionality dim = WorldDimensionality::dim2D;
	typedef DimensionalityTraits< dim > dim_traits;

public:
	struct state
	{
		dim_traits::angular_velocity_t	ang_vel;

		enum {
			NumScalarInputs = 1,	// TODO: 2D
		};

		inline void as_nn_input(std::vector< double >& input) const
		{
			// TODO: Currently 2D specific
			input.push_back(ang_vel);
		}

		state()
		{}

		state(ship_state< dim > const& ss)
		{
			ang_vel = dim_traits::inv_transform_dir(ss.ang_velocity, ss.orientation);
		}

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	};

	// The performance data from a single problem instance
	struct performance_data
	{
		int timesteps;
		double average_speed;
		int failed_to_oppose;
		bool unfit;

		performance_data(): timesteps(0), average_speed(0), failed_to_oppose(0), unfit(false)
		{}
	};

	enum {
		MaxTimesteps = 100,
	};

	// Transitions to next state based on implementation of policy.
	// Returns true if the resulting state is termination state (ie. the instance is completed).
	// This function is responsible for updating the performance data.
	struct state_transition_fn
	{
		// TODO: For now, hard coding results of policy implementation to be resultant force/torque pair
		bool operator() (
			thruster_base::thruster_activation const& ta,
			ship_state< dim > const& ship_st,	// (Before update)
			//dim_traits::force_t const& res_force, dim_traits::torque_t const& res_torque,
			//bot_action const& action,
			state& st, performance_data& pd) const
		{
/* TODO:
			if(ship_st.activation_contains_null(ta))
			{
				// Incompatible thrusters were fired, meaning this nn is unacceptable solution, so bail now
				pd.unfit = true;
				return true;
			}
*/

			// TODO: Hardcoding for now, based on square_minimal thruster config
			double thruster_power = 0.02;

			dim_traits::force_t res_force;
			dim_traits::torque_t res_torque;
			boost::tie(res_force, res_torque) = ship_st.thruster_sys.cfg.calc_resultants(ta, ship_st.c_of_mass);

#if 0
			std::cout << "(" << st.lin_vel[0] << ", " << st.lin_vel[1] << "; " << st.ang_vel[2] << ") -> (" << 
				(res_force[0]/*force2d_world[0]*/ * thruster_power) << ", " << 
				(res_force[1]/*force2d_world[1]*/ * thruster_power) << "; " << 
				(res_torque[2] * thruster_power) << ")" << std::endl;
#endif

			if(res_torque * st.ang_vel > 0.0 || st.ang_vel == 0.0 && res_torque != 0.0)
			{
				++pd.failed_to_oppose;
			}

			st.ang_vel += res_torque * thruster_power;

			pd.average_speed += magnitude(st.ang_vel);

			++pd.timesteps;

			// If sufficiently close to a full stop, or we've hit the maximum number of timesteps,
			// terminate the problem instance.
			return /*st.ang_vel.norm() < AcceptableAngularSpeed ||*/ pd.timesteps >= MaxTimesteps;
		}
	};

	typedef basic_real_fitness fitness_t;

	template < typename EpochData >
	struct avg_speed_fitness_fn
	{
		fitness_t operator() (EpochData const& ed) const
		{
			fitness_t fitness(0.0);
			for(auto const& inst_data: ed)
			{
				double avg_speed = inst_data.perf.average_speed / inst_data.perf.timesteps;
				double prop_avg_speed = std::min< double >(avg_speed / magnitude(inst_data.initial_st.ang_vel), 1.0);
				fitness += fitness_t(1.0 - prop_avg_speed);
			}
			return fitness / ed.size();
		}
	};

	template < typename EpochData >
	struct w_opposition_fitness_fn
	{
		fitness_t operator() (EpochData const& ed) const
		{
			fitness_t fitness(0.0);
			for(auto const& inst_data: ed)
			{
				fitness += fitness_t(1.0 - (double)inst_data.perf.failed_to_oppose / inst_data.perf.timesteps);
			}
			return fitness / ed.size();
		}
	};

	template < typename EpochData >
	struct slowdown_fitness_fn
	{
		fitness_t operator() (EpochData const& ed) const
		{
			fitness_t fitness(0.0);
			for(auto const& inst_data: ed)
			{
				// How about some formulation whereby as speed tends to zero, the 'need' to decelerate diminishes
				// asymptotically, such that at speeds near zero, a decelerating behaviour is not really any fitter
				// than one which keeps speed unchanged. But what about actually accelerating..? Generally, point is
				// it seems like a good idea to try to get rid of this singularity around an initial zero speed.

				double delta_angular_speed = magnitude(inst_data.current_st.ang_vel) - magnitude(inst_data.initial_st.ang_vel);
				double angular_deceleration = -delta_angular_speed;
				double angular_deceleration_rate = angular_deceleration / inst_data.perf.timesteps;

				const double p_ang = 0.04 * 0.25;	// Make ideal deceleration rate map to 4 so the sigmoid result is close to 1

				double sigm_ang = 1.0 / (1.0 + std::exp(-angular_deceleration_rate / p_ang));

				fitness += fitness_t(sigm_ang);
			}
			return fitness / ed.size();
		}
	};

	template < typename EpochData >
	struct overall_right_dir_fitness_fn
	{
		fitness_t operator() (EpochData const& ed) const
		{
			fitness_t fitness = 0.0;
			for(auto const& inst_data: ed)
			{
				if(magnitude(inst_data.current_st.ang_vel) - magnitude(inst_data.initial_st.ang_vel) < 0.00001)
				{
					fitness += (magnitude(inst_data.initial_st.ang_vel) < 0.00001 ? 1.0 : 0.0);
				}
				else
				{
					int delta_dir = (inst_data.current_st.ang_vel - inst_data.initial_st.ang_vel > 0.0) ? 1 : -1;
					int desired_dir = (0.0 - inst_data.initial_st.ang_vel > 0.0) ? 1 : -1;
					fitness += (delta_dir == desired_dir ? 1.0 : 0.0);
				}
			}
			return fitness / ed.size();
		}
	};

	template < typename EpochData >
	struct fitness_fn
	{
		fitness_t operator() (EpochData const& ed) const
		{
			return avg_speed_fitness_fn< EpochData >() (ed);
		}
	};

	template< typename RGen >
	struct input_state_fn
	{
		state operator() (RGen& rgen) const
		{
			boost::random::uniform_real_distribution<> dist(-1.0, 1.0);

			state st;
			st.ang_vel = dist(rgen);// * 0.03;
			return st;
		}
	};
};


#endif


