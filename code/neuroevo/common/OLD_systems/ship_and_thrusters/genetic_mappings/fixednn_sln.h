// fixednn_sln.h

#ifndef __SAT_FIXED_NN_SLN_H
#define __SAT_FIXED_NN_SLN_H

#include "../../fixed_neural_net.h"

#include <doublefann.h>
#include <fann_cpp.h>

#include <vector>


namespace sat {

	template < typename System >
	struct fixednn_solution_base
	{
		typedef System system_t;
		typedef typename system_t::solution_result result_t;

		enum {
			NumNNOutputs = 4,		// TODO: !!!!!!!!!!!!!!!!!!!
		};

		inline result_t process(std::vector< double > const& nn_inputs)
		{
			double* nn_outputs = nn.run((double*)&nn_inputs[0]);

			result_t result(NumNNOutputs);
			for(size_t i = 0; i < NumNNOutputs; ++i)
			{
				result[i] = nn_outputs[i] > 0.0;
			}
			return result;
		}

		FANN::neural_net nn;
	};

	template <
		typename System,
		typename Input
	>
	struct full_stop_fixednn_solution: public fixednn_solution_base< System >
	{
		typedef System system_t;
		typedef Input input_t;

		typedef typename system_t::dim_traits_t dim_traits_t;
		typedef typename system_t::solution_result result_t;

		enum {
			NumNNOutputs = 4,		// TODO: !!!!!!!!! Depends on thruster config !!!!!!!!!!!!!!
			NumNNInputs =
				num_components< dim_traits_t::velocity_t >::val +
				num_components< dim_traits_t::angular_velocity_t >::val + 
				num_components< dim_traits_t::orientation_t >::val,
			NumNNLayers = 4,
			NumNNHidden = (NumNNInputs + NumNNOutputs) / 2,
		};

		full_stop_fixednn_solution()
		{
			std::vector< unsigned int > layer_neurons((size_t)NumNNLayers, (unsigned int)NumNNHidden);
			layer_neurons[0] = NumNNInputs;
			layer_neurons[NumNNLayers - 1] = NumNNOutputs;
			nn.create_standard_array(NumNNLayers, &layer_neurons[0]);

			nn.set_activation_steepness_hidden(1.0);
			nn.set_activation_steepness_output(1.0);

			nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
			nn.set_activation_function_output(FANN::THRESHOLD);
		}

		static inline std::vector< double > map_inputs(input_t const& input)
		{
			std::vector< double > nn_inputs(NumNNInputs);
			std::vector< double >::iterator it = nn_inputs.begin();
			get_components(input.lin_vel, it);
			get_components(input.ang_vel, it);
			get_components(input.orient, it);
			assert(it == nn_inputs.end());
			return nn_inputs;
		}

		inline result_t operator() (input_t const& input)
		{
			return process(map_inputs(input));
		}
	};

	template <
		typename System,
		typename Input
	>
	struct angular_full_stop_fixednn_solution: public fixednn_solution_base< System >
	{
		typedef System system_t;
		typedef Input input_t;

		typedef typename system_t::dim_traits_t dim_traits_t;
		typedef typename system_t::solution_result result_t;

		enum {
			NumNNOutputs = 4,		// TODO: !!!!!!!!! Depends on thruster config !!!!!!!!!!!!!!
			NumNNInputs =
				num_components< dim_traits_t::angular_velocity_t >::val,
			NumNNLayers = 3,
			NumNNHidden = (NumNNInputs + NumNNOutputs) / 2,
		};

		angular_full_stop_fixednn_solution()
		{
			std::vector< unsigned int > layer_neurons((size_t)NumNNLayers, (unsigned int)NumNNHidden);
			layer_neurons[0] = NumNNInputs;
			layer_neurons[NumNNLayers - 1] = NumNNOutputs;
			nn.create_standard_array(NumNNLayers, &layer_neurons[0]);

			nn.set_activation_steepness_hidden(1.0);
			nn.set_activation_steepness_output(1.0);

			nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
			nn.set_activation_function_output(FANN::THRESHOLD);
		}

		static inline std::vector< double > map_inputs(input_t const& input)
		{
			std::vector< double > nn_inputs(NumNNInputs);
			std::vector< double >::iterator it = nn_inputs.begin();
			get_components(input.ang_vel, it);
			assert(it == nn_inputs.end());
			return nn_inputs;
		}

		inline result_t operator() (input_t const& input)
		{
			return process(map_inputs(input));
		}
	};

	template <
		typename System,
		typename Input
	>
	struct target_orientation_fixednn_solution: public fixednn_solution_base< System >
	{
		typedef System system_t;
		typedef Input input_t;

		typedef typename system_t::dim_traits_t dim_traits_t;
		typedef typename system_t::solution_result result_t;

		enum {
			NumNNOutputs = 4,		// TODO: !!!!!!!!! Depends on thruster config !!!!!!!!!!!!!!
			NumNNInputs =
				num_components< dim_traits_t::orientation_t >::val +
				num_components< dim_traits_t::angular_velocity_t >::val,
			NumNNLayers = 4,
			NumNNHidden = (NumNNInputs + NumNNOutputs) / 2,
		};

		target_orientation_fixednn_solution()
		{
			std::vector< unsigned int > layer_neurons((size_t)NumNNLayers, (unsigned int)NumNNHidden);
			layer_neurons[0] = NumNNInputs;
			layer_neurons[NumNNLayers - 1] = NumNNOutputs;
			nn.create_standard_array(NumNNLayers, &layer_neurons[0]);

			nn.set_activation_steepness_hidden(1.0);
			nn.set_activation_steepness_output(1.0);

			nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
			nn.set_activation_function_output(FANN::THRESHOLD);
		}

		static inline std::vector< double > map_inputs(input_t const& input)
		{
			std::vector< double > nn_inputs(NumNNInputs);
			std::vector< double >::iterator it = nn_inputs.begin();
			get_components(input.orient, it);
			get_components(input.ang_vel, it);
			assert(it == nn_inputs.end());
			return nn_inputs;
		}

		inline result_t operator() (input_t const& input)
		{
			return process(map_inputs(input));
		}
	};

//	template < typename System >
//	using fixed_nn_mapping = fixed_neural_net< System, fixednn_solution< System,  > >;

}


#endif


