// fixednn_sln.h

#ifndef __NAC_FIXED_NN_SLN_H
#define __NAC_FIXED_NN_SLN_H

#include "../../fixed_neural_net.h"

#include <doublefann.h>
#include <fann_cpp.h>

#include <vector>

/*
Agent is represented by a neural network with:
2 * Size ^ Dimensions inputs (one pair for each board location, where (0, 0) = unplayed, (1, 0) = player marked, (0, 1) = opponent marked.
Some predetermined number of hidden nodes.
Size ^ Dimensions outputs (only one of which should be activated - see bookmark on mutually exclusive classes output.
*/

namespace nac {

	template < typename System >
	struct fixednn_solution_base
	{
		typedef System system_t;
		typedef typename system_t::board_state state_t;
		typedef typename system_t::solution_result result_t;

		enum {
			NumNNOutputs = system_t::NumSquares,
		};

		inline result_t process(state_t const& st, std::vector< double > const& nn_inputs)
		{
			double* nn_outputs = nn.run((double*)&nn_inputs[0]);

			// TODO: try softmax activation (see bookmarked webpage)
			boost::optional< size_t > activated_output;
			for(size_t i = 0; i < NumNNOutputs; ++i)
			{
				double const threshold = 0.0;	// 0 = always activated
				if(nn_outputs[i] >= threshold && (!activated_output || nn_outputs[i] > nn_outputs[*activated_output]))
				{
					activated_output = i;
				}
			}

			// Was an output activated?
			if(!activated_output)
			{
				return result_t();
			}

			// If we activated an output corresponding to a non-vacant square, signify failure
			if(st.by_index(*activated_output) != system_t::SquareState::Unplayed)
			{
				return result_t();
			}

			// Successfully chose valid square to play
			return result_t(*activated_output);
		}

		FANN::neural_net nn;
	};

	template <
		typename System,
		typename Input
	>
	struct default_fixednn_solution: public fixednn_solution_base< System >
	{
		typedef System system_t;
		typedef Input input_t;

		typedef typename system_t::solution_result result_t;

		enum {
			NumSquares = system_t::NumSquares,
			NumNNOutputs = NumSquares,
			NumNNInputs = 2 * NumNNOutputs,
			NumNNLayers = 3,
			NumNNHidden = (NumNNInputs + NumNNOutputs) / 2,
		};

		default_fixednn_solution()
		{
			std::vector< unsigned int > layer_neurons((size_t)NumNNLayers, (unsigned int)NumNNHidden);
			layer_neurons[0] = NumNNInputs;
			layer_neurons[NumNNLayers - 1] = NumNNOutputs;
			nn.create_standard_array(NumNNLayers, &layer_neurons[0]);

			nn.set_activation_steepness_hidden(1.0);
			nn.set_activation_steepness_output(1.0);

			nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
			nn.set_activation_function_output(FANN::SIGMOID_STEPWISE);// THRESHOLD);
		}

		static inline std::vector< double > map_inputs(input_t const& input)
		{
			std::vector< double > nn_inputs(NumNNInputs);

			system_t::Player player = (input.num_played_squares() % 2 == 0) ? system_t::Player::Crosses : system_t::Player::Noughts;
			for(size_t i = 0; i < NumSquares; ++i)
			{
				nn_inputs[i * 2] = input.by_index(i) == system_t::player_marker(player) ? 1.0 : 0.0;
				nn_inputs[i * 2 + 1] = input.by_index(i) == system_t::player_marker(system_t::other_player(player)) ? 1.0 : 0.0;
			}
			return nn_inputs;
		}

		inline result_t operator() (input_t const& input)
		{
			return process(input, map_inputs(input));
		}
	};

//	template < typename System >
//	using fixed_nn_mapping = fixed_neural_net< System, fixednn_solution< System,  > >;

}


#endif


