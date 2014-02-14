// target_orientation.h

#ifndef __TARGET_ORIENTATION_H
#define __TARGET_ORIENTATION_H

#include "problem_base.h"

#include "Eigen/Dense"

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/math/constants/constants.hpp>

#include <vector>
#include <cmath>
#include <algorithm>

#if 1
#include <iostream>
#endif


using Eigen::Vector3d;


class target_orientation
{
public:
	struct state
	{
		// TODO: 2D
		double		delta;		// Angle between current and target orientations, [-Pi, Pi)
		double		omega;		// Current angular speed

		enum {
			NumScalarInputs = 2,
		};

		inline void as_nn_input(std::vector< double >& input) const
		{
			input.push_back(delta);
			input.push_back(omega);
		}
	};

	// The performance data from a single problem instance
	struct performance_data
	{
		int timesteps;
		double average_delta;

		performance_data(): timesteps(0), average_delta(0)
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
		bool operator() (Vector3d const& res_force, Vector3d const& res_torque, state& st, performance_data& pd) const
		{
			// TODO: Hardcoding for now, based on square_minimal thruster config
			double thruster_power = 0.02;

			st.omega += (res_torque * thruster_power)[2];
			st.delta -= st.omega;
			// Keep delta in the range [-Pi, Pi)
			st.delta = std::fmod(st.delta + boost::math::double_constants::pi, 2.0 * boost::math::double_constants::pi) -
				boost::math::double_constants::pi;

			pd.average_delta += std::abs(st.delta);
			++pd.timesteps;

			return pd.timesteps >= MaxTimesteps;
		}
	};

	typedef double fitness_t;

	template < typename EpochData >
	struct fitness_fn
	{
		fitness_t operator() (EpochData const& ed) const
		{
			fitness_t fitness = 0.0;
			for(auto const& inst_data: ed)
			{
				double avg_delta = inst_data.perf.average_delta / inst_data.perf.timesteps;
				fitness += (boost::math::double_constants::pi - avg_delta) / boost::math::double_constants::pi;

				Component for reaching acceptably small delta with acceptably small omega in as short a time as possible.

				Component for minimising number of sign changes of omega up to the above achievement.
			}
			return fitness / ed.size();
		}
	};

	template< typename RGen >
	struct input_state_fn
	{
		state operator() (RGen& rgen) const
		{
			boost::random::uniform_real_distribution<> dist(-boost::math::double_constants::pi, boost::math::double_constants::pi);

			state st;
			st.delta = dist(rgen);
			st.omega = 0.0;
			return st;
		}
	};
};


#endif


