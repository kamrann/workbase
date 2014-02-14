// simple_force_torque.h

#ifndef __SIMPLE_FORCE_TORQUE_H
#define __SIMPLE_FORCE_TORQUE_H

#include "problem_base.h"

#include "Eigen/Dense"

#include <boost/math/constants/constants.hpp>

#include <vector>


using Eigen::Vector3d;


class simple_force_torque
{
public:
	// Describes a given instance of the problem, ie. a {requested force, requested torque} pair
	struct state//problem_instance_data
	{
		Vector3d	req_force;
		Vector3d	req_torque;

		enum {
			NumScalarInputs = 3,	// TODO: 2D
		};

		inline void as_nn_input(std::vector< double >& input) const
		{
			// TODO: Currently 2D specific
			input.push_back(req_force[0]);
			input.push_back(req_force[1]);
//			input.push_back(req_force[2]);
//			input.push_back(req_torque[0]);
//			input.push_back(req_torque[1]);
			input.push_back(req_torque[2]);
		}
	};

	// The performance data from a single problem instance
	struct performance_data
	{
		Vector3d	act_force;
		Vector3d	act_torque;
	};

	// Transitions to next state based on implementation of policy.
	// Returns true if the resulting state is termination state (ie. the instance is completed).
	// This function is responsible for updating the performance data.
	struct state_transition_fn
	{
		// TODO: For now, hard coding results of policy implementation to be resultant force/torque pair
		bool operator() (Vector3d const& res_force, Vector3d const& res_torque, state& st, performance_data& pd) const
		{
			pd.act_force = res_force;
			pd.act_torque = res_torque;

			// This problem class is instantaneous, so always terminate the instance after first update
			return true;
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
				fitness += 1.0 - calc_instance_error(inst_data.initial_st, inst_data.current_st, inst_data.perf);
			}
			return fitness / ed.size();
		}

	private:
		double calc_instance_error(state const& requested, state const&, performance_data const& actual) const
		{
			// map to [0, 1]

			//double cos_torque_angle = actual_torque.dot(desired.desired_torque) / (actual_torque.norm() * desired.desired_torque.norm());
			// TODO: should we just use the cosine value, or a linear measure (ie. cos-1(cos_torque_angle) / 2*Pi) ?
			double torque_error = 1.0 - 1.0 / (1.0 + (actual.act_torque - requested.req_torque).norm());
			//double torque_error = 0.5 - cos_torque_angle * 0.5;

			//double cos_force_angle = actual_force.dot(desired.desired_force) / (actual_force.norm() * desired.desired_force.norm());
			// TODO: as above
			double force_error = 1.0 - 1.0 / (1.0 + (actual.act_force - requested.req_force).norm());
			//double force_error = 0.5 - cos_force_angle * 0.5;

			// TODO: for now, just weight the two equally
			return force_error * 0.5 + torque_error * 0.5;
		}
	};

	template< typename RGen >
	struct input_state_fn
	{
		state operator() (RGen& rgen) const
		{
			boost::random::uniform_real_distribution<> dist(0.0, 1.0);

			state st;
			double theta = dist(rgen) * 2.0 * boost::math::double_constants::pi;
			st.req_force = Vector3d(0, 0, 0);//Vector3d(cos(theta), sin(theta), 0);
			st.req_torque = Vector3d(0, 0, dist(rgen) < 0.5 ? 2.0 : -2.0);
			return st;
		}
	};
};


#endif


