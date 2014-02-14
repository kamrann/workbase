// full_stop.h

#ifndef __FULL_STOP_H
#define __FULL_STOP_H

#include "problem_base.h"

#include "Eigen/Dense"

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/math/constants/constants.hpp>

#include <vector>
#include <cmath>

#if 1
#include <iostream>
#endif


using Eigen::Vector3d;


const double AcceptableLinearSpeed = 0.00001f;
const double AcceptableAngularSpeed = 0.00001f;


class full_stop
{
public:
	// Describes a given instance of the problem, ie. an initial linear and angular velocity
	struct state//problem_instance_data
	{
		double		facing;	// TODO: 2D specific

		Vector3d	lin_vel;
		Vector3d	ang_vel;

		enum {
			NumScalarInputs = 3,//4,	// TODO: 2D
		};

		inline void as_nn_input(std::vector< double >& input) const
		{
			// TODO: Currently 2D specific
			//Taken out this input, so lin_vel input needs to be changed to always be in local coord system.
			//	Also, see what happens if we halve the outputs by coupling thrusters into rotational pairs (ie. inhibit linear resultants)
//			input.push_back(facing);
			input.push_back(lin_vel[0]);
			input.push_back(lin_vel[1]);
//			input.push_back(lin_vel[2]);
//			input.push_back(ang_vel[0]);
//			input.push_back(ang_vel[1]);
			input.push_back(ang_vel[2]);
		}
	};

	// The performance data from a single problem instance
	struct performance_data
	{
		int timesteps;

		performance_data(): timesteps(0)
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
			double thruster_power = 0.001;

			// Transform resultant force vector from bot local space to world space, using existing state's 'facing' value
//			Vector2d force2d(res_force[0], res_force[1]);
//			Vector2d force2d_world = Eigen::Rotation2D< double >(st.facing) * force2d;

#if 0
			std::cout << "(" << st.lin_vel[0] << ", " << st.lin_vel[1] << "; " << st.ang_vel[2] << ") -> (" << 
				(res_force[0]/*force2d_world[0]*/ * thruster_power) << ", " << 
				(res_force[1]/*force2d_world[1]*/ * thruster_power) << "; " << 
				(res_torque[2] * thruster_power) << ")" << std::endl;
#endif

			// Update the bot's world space linear velocity vector
			st.lin_vel += res_force/*Vector3d(force2d_world[0], force2d_world[1], 0.0)*/ * thruster_power;
			
			// For angular in 2D, we don't need to transform the torque
			st.ang_vel += res_torque * thruster_power;

			// Now update the bot's world space facing, using the new angular velocity
			st.facing += st.ang_vel[2];
			st.facing = std::fmod(st.facing, 2.0 * boost::math::double_constants::pi);

			//
			Vector2d tmp = Eigen::Rotation2D< double >(-st.ang_vel[2]) * Vector2d(st.lin_vel[0], st.lin_vel[1]);
			st.lin_vel = Vector3d(tmp[0], tmp[1], 0.0);
			//

			++pd.timesteps;

			// If sufficiently close to a full stop, or we've hit the maximum number of timesteps,
			// terminate the problem instance.
			return (st.lin_vel.norm() < AcceptableLinearSpeed && st.ang_vel.norm() < AcceptableAngularSpeed) ||
				pd.timesteps >= MaxTimesteps;
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
				// How about some formulation whereby as speed tends to zero, the 'need' to decelerate diminishes
				// asymptotically, such that at speeds near zero, a decelerating behaviour is not really any fitter
				// than one which keeps speed unchanged. But what about actually accelerating..? Generally, point is
				// it seems like a good idea to try to get rid of this singularity around an initial zero speed.

				double delta_linear_speed = inst_data.current_st.lin_vel.norm() - inst_data.initial_st.lin_vel.norm();
				double linear_deceleration = -delta_linear_speed;

				//double prop_lin_dec = linear_deceleration / inst_data.initial_st.lin_vel.norm();

				double linear_deceleration_rate = /*prop_lin_dec/*/linear_deceleration / inst_data.perf.timesteps;

				double delta_angular_speed = inst_data.current_st.ang_vel.norm() - inst_data.initial_st.ang_vel.norm();
				double angular_deceleration = -delta_angular_speed;

				//double prop_ang_dec = angular_deceleration / inst_data.initial_st.ang_vel.norm();

				double angular_deceleration_rate = /*prop_ang_dec/*/angular_deceleration / inst_data.perf.timesteps;

				const double p_lin = 0.002 * 0.25;	// Make ideal deceleration rate map to 4 so the sigmoid result is close to 1
				const double p_ang = 0.002 * 0.25;	// Make ideal deceleration rate map to 4 so the sigmoid result is close to 1
				// These currently same since with square_minimal, lin and ang potential are similar

				// Try combining the two fitness components by taking product of their sigmoids
				double sigm_lin = 1.0 / (1.0 + std::exp(-linear_deceleration_rate / p_lin));
				double sigm_ang = 1.0 / (1.0 + std::exp(-angular_deceleration_rate / p_ang));

				/////////////////////
				double right_dir_fitness_contrib = 0.5;
				if(inst_data.current_st.ang_vel.norm() - inst_data.initial_st.ang_vel.norm() < 0.00001)
				{
					sigm_ang = right_dir_fitness_contrib * ((inst_data.initial_st.ang_vel.norm() < 0.00001) ? 1.0 : 0.0) +
						(1.0 - right_dir_fitness_contrib) * sigm_ang;
				}
				else
				{
					int delta_dir = (inst_data.current_st.ang_vel[2] - inst_data.initial_st.ang_vel[2] > 0.0) ? 1 : -1;
					int desired_dir = (0.0 - inst_data.initial_st.ang_vel[2] > 0.0) ? 1 : -1;
					sigm_ang = right_dir_fitness_contrib * (delta_dir == desired_dir ? 1.0 : 0.0) + (1.0 - right_dir_fitness_contrib) * sigm_ang;
				}
				/////////////////////

				fitness += sigm_lin * sigm_ang;
			}
			return fitness / ed.size();
		}
	};

	template< typename RGen >
	struct input_state_fn
	{
		state operator() (RGen& rgen) const
		{
			boost::random::uniform_real_distribution<> dist(-1.0, 1.0);

			state st;
			st.facing = dist(rgen) * boost::math::double_constants::pi;
			st.lin_vel = Vector3d(0, 0, 0);//dist(rgen), dist(rgen), 0) * 100.0;
			st.ang_vel = Vector3d(0, 0, dist(rgen)) * 0.03;
			return st;
		}
	};
};


#endif


