// angular_full_stop.h

#ifndef __ANGULAR_FULL_STOP_H
#define __ANGULAR_FULL_STOP_H

#include "ga/fitness.h"
#include "ga/composite_fitness_fn.h"
#include "ga/shared_objective_fn_data_components.h"

#include "problem_base.h"
#include "util/dimensionality.h"
#include "thrusters/ship_state.h"

#include "doublefann.h"
#include "fann_cpp.h"

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/tuple/tuple.hpp>

#include <cmath>
#include <algorithm>

#if 1
#include <iostream>
#endif


template < WorldDimensionality Dim >
class angular_full_stop
{
public:
	static WorldDimensionality const dim = Dim;
	typedef DimensionalityTraits< dim > dim_traits;

public:
	/*
	Structure for state of a single agent.
	*/
	typedef ship_state< dim > agent_state;
/*
	struct agent_state
	{
		typename dim_traits::angular_velocity_t	ang_velocity;

		agent_state(): ang_velocity(zero_val< dim_traits::angular_velocity_t >())
		{}

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	};
*/
	/*
	This structure represents the state of the problem domain/environment, excluding that of the contained agents.
	*/
	struct envt_state
	{};

	/*
	This is the state known to a given agent, and represents all the information available to that agent to make decisions.
	*/
	struct agent_sensor_state
	{
		typename dim_traits::angular_velocity_t ang_vel;

		agent_sensor_state(agent_state const& a)
		{
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


	/*
	Agent decision represents the choice of action made by an agent at a given update.
	*/
	typedef thruster_base::thruster_activation agent_decision;


	/*
	Now, we can define (maybe should be outside of this class??) various agent implementations to try different decision making
	processes. An agent implementation is simply a mapping from agent_sensor_state to agent_decision.
	TODO: But we also need to specify somewhere (here?) how it can be encoded into a genome, or decoded from a genome.
	*/
	template<
		size_t NumNNLayers,
		size_t NumNNHidden
	>
	struct nn_agent_impl
	{
		enum {
			NumNNInputs = num_components< dim_traits::angular_velocity_t >::val
		};

		size_t num_thrusters;
		//boost::shared_ptr< FANN::neural_net > nn;
		FANN::neural_net nn;

		nn_agent_impl(size_t thrusters): num_thrusters(thrusters)//, nn(new FANN::neural_net)
		{
			std::vector< unsigned int > layer_neurons(NumNNLayers, NumNNHidden);
			layer_neurons[0] = NumNNInputs;
			layer_neurons[NumNNLayers - 1] = num_thrusters;
			nn.create_standard_array(NumNNLayers, &layer_neurons[0]);

			nn.set_activation_steepness_hidden(1.0);
			nn.set_activation_steepness_output(1.0);

			nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
			nn.set_activation_function_output(FANN::THRESHOLD);
		}

		static inline size_t get_genome_length(size_t thrusters)
		{
			nn_agent_impl temp(thrusters);
			return temp.nn.get_total_connections();
		}

		inline agent_decision operator() (agent_sensor_state const& st)// const
		{
			std::vector< double > nn_inputs(NumNNInputs);
			get_components(st.ang_vel, nn_inputs.begin());
			double* nn_outputs = nn.run(&nn_inputs[0]);

			thruster_base::thruster_activation ta(num_thrusters);
			for(size_t ithr = 0; ithr < num_thrusters; ++ithr)
			{
				ta[ithr] = (nn_outputs[ithr] > 0.5);
			}
			return ta;
		}
	};


	template < typename RGen >
	static inline void init_state(boost::shared_ptr< thruster_config< dim > > t_cfg, // TODO: not sure whether this should be passed in or specified elsewhere.
		// it is problem specific, but currently problem class has no state...
		size_t const num_agents, state& st, RGen& rgen)
	{
		boost::random::uniform_real_distribution<> dist(-1.0, 1.0);

		// Initial state is just a random angular velocity
		st.agents.clear();
		st.agents.resize(num_agents);

		// TODO: For now, give every agent the same starting state for any given trial
		// This will need to be removed eventually as not feasible for shared environment training
		dim_traits::angular_velocity_t init_vel = random_val< dim_traits::angular_velocity_t >(rgen);
		//

		for(auto& a: st.agents)
		{
			a.thruster_cfg = t_cfg;
			//a.ang_vel
			a.ang_velocity = init_vel;//random_val< dim_traits::angular_velocity_t >(rgen);
		}
	}

	static inline void update_state(state& st, agent_decision const decisions[])
	{
		size_t iagent = 0;
		for(auto& a_state: st.agents)
		{
			thruster_base::thruster_activation const& ta = decisions[iagent];
			double const thruster_power = 0.02;

			typename dim_traits::force_t res_force;
			typename dim_traits::torque_t res_torque;
			boost::tie(res_force, res_torque) = a_state.thruster_cfg->calc_resultants(ta, a_state.c_of_mass);

// ??			a_state.lin_velocity += res_force * thruster_power;
			a_state.ang_velocity += res_torque * thruster_power;

// ??			a_state.position += 

			++iagent;
		}
	}

	typedef basic_real_fitness fitness_t;

/*	template < typename TrialData >
	struct avg_speed_fitness_fn
	{
		struct fdata
		{
			double avg_spd;
			size_t timesteps;

			fdata(): avg_spd(0.0), timesteps(0)
			{}
		};

		// Note these states are resulting states after effecting agent action and updating
		void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, fdata& fd)
		{
			fd.avg_spd += magnitude(agent_st.ang_velocity);
			++fd.timesteps;
		}

		fitness_t evaluate(fdata const& fd, TrialData const& td) const
		{
			double avg_speed = fd.avg_spd / fd.timesteps;
			double prop_avg_speed = std::min< double >(avg_speed / magnitude(td.initial_st.ang_velocity), 1.0);
			return fitness_t(1.0 - prop_avg_speed);
		}
	};
*/
	struct avg_speed_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector< avg_angular_speed_ofd > dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			double prop_avg_speed = std::min< double >(ofd.avg_ang_speed / magnitude(td.initial_st.ang_velocity), 1.0);
			return obj_value_t(1.0 - prop_avg_speed);
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

	struct slowdown_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector< stopped_rotating_ofd > dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			// TODO: as in above method
			double const IdealDecelRate = 0.04;
			double const SigmoidLimit = 4.0;

			double end_speed = ofd.stopped_rotating ? 0.0 : magnitude(td.final_st.ang_velocity);
			double accel = end_speed - magnitude(td.initial_st.ang_velocity);
			size_t timesteps = ofd.stopped_rotating ? ofd.timesteps_to_stopped_rotating : ofd.timesteps;
			double decel_rate = -accel / timesteps;

			// NOTE: If not normalized, could potentially be +ve/-ve and any size
			// Roulette wheel selection requires +ve values with meaningful distribution
			double sigm = 1.0 / (1.0 + std::exp(-decel_rate * SigmoidLimit / IdealDecelRate));

			return obj_value_t(sigm);
		}

/*		fitness_t operator() (EpochData const& ed) const
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
*/	};

	struct minimize_resultant_force_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector< avg_resultant_force_ofd< dim > > dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			return obj_value_t(-ofd.avg_resultant_force);
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
};


#endif


