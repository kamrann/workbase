// fixed_neural_net.h

#ifndef __NAC_FIXED_NEURAL_NET_H
#define __NAC_FIXED_NEURAL_NET_H

#include <doublefann.h>
#include <fann_cpp.h>

#include <vector>
#include <cassert>

/*
Agent is represented by a neural network with:
2 * Size ^ Dimensions inputs (one pair for each board location, where (0, 0) = unplayed, (1, 0) = player marked, (0, 1) = opponent marked.
Some predetermined number of hidden nodes.
Size ^ Dimensions outputs (only one of which should be activated - see bookmark on mutually exclusive classes output.

Genome is a vector of floats representing neuron weights.
*/

// TODO: Perhaps want to separate Solution Impl Type (first part above) from Genetic Encoding, so we can specify a Solution Impl Type
// (as above for example) and then specify multiple possible genetic encodings for it.

template <
	typename System,
	typename Input
>
class fixed_neural_net
{
public:
	typedef System system_t;
	typedef Input input_t;

	enum {
		Dimensions = system_t::Dimensions,
		Size = system_t::Size,
	};

	struct genome: public std::vector< double >
	{
		inline size_t length() const
		{
			return size();
		}

		// Lexicographical ordering
		inline bool operator< (genome const& other) const
		{
			for(size_t i = 0; i < length(); ++i)
			{
				if((*this)[i] < other[i])
				{
					return true;
				}
				else if((*this)[i] > other[i])
				{
					return false;
				}
			}
			return false;
		}

		//friend std::ostream& operator<< (std::ostream& out, genome const& gn);
	};

	typedef double genome_diff_t;

	static inline genome_diff_t genome_difference(genome const& gn1, genome const& gn2)
	{
		assert(gn1.length() == gn2.length());

		genome_diff_t diff = 0.0;
		for(size_t i = 0; i < gn1.length(); ++i)
		{
			double gene_diff = gn2[i] - gn1[i];
			diff += gene_diff * gene_diff;
		}
		return std::sqrt(diff);

		// TODO: Some kind of normalization?
		// Weight values are unbounded, but seems like a weight change may be more volatile in one gene than another.
		// This could lead to genes where the precise value was not so important to the network output having a large effect
		// on the difference measure, which seems bad.
	}

	struct solution
	{
		enum {
			NumSquares = system_t::NumSquares,
			NumNNOutputs = NumSquares,
			NumNNInputs = 2 * NumNNOutputs,
			NumNNLayers = 3,
			NumNNHidden = (NumNNInputs + NumNNOutputs) / 2,
		};

//		typedef typename scenario_t::solution_input input_t;
		typedef typename system_t::solution_result result_t;

		FANN::neural_net nn;

		solution()
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

		inline result_t operator() (input_t const& input)
		{
			std::vector< double > nn_inputs(NumNNInputs);

			system_t::Player player = (input.num_played_squares() % 2 == 0) ? system_t::Player::Crosses : system_t::Player::Noughts;
			for(size_t i = 0; i < NumSquares; ++i)
			{
				nn_inputs[i * 2] = input.by_index(i) == system_t::player_marker(player) ? 1.0 : 0.0;
				nn_inputs[i * 2 + 1] = input.by_index(i) == system_t::player_marker(system_t::other_player(player)) ? 1.0 : 0.0;
			}
			double* nn_outputs = nn.run(&nn_inputs[0]);

			// TODO: try softmax activation (see bookmarked webpage)
			boost::optional< size_t > activated_output;
			for(size_t i = 0; i < NumNNOutputs; ++i)
			{
				double const threshold = 0.0;	// 0 = always activated
				if(nn_outputs[i] >= threshold && (!activated_output || nn_outputs[i] > nn_outputs[*activated_output]))
				{
					activated_output = i;
				}

/*				if(nn_outputs[i] > 0.5)
				{
					if(activated_output)
					{
						// Multiple output neurons activated, signify failed result
						return result_t();
					}
					else
					{
						activated_output = i;
					}
				}
*/			}

			// Was an output activated?
			if(!activated_output)
			{
				return result_t();
			}

			// If we activated an output corresponding to a non-vacant square, signify failure
			if(input.by_index(*activated_output) != system_t::SquareState::Unplayed)
			{
				return result_t();
			}

			// Successfully chose valid square to play
			return result_t(*activated_output);
		}
	};

	static inline size_t get_genome_length()
	{
		solution temp;
		return temp.nn.get_total_connections();
	}

	template < typename RGen >
	static inline genome generate_random_genome(/*size_t const length,*/ RGen& rgen)
	{
		solution temp;
		size_t num_weights = get_genome_length();
		std::vector< FANN::connection > conns(num_weights);
		temp.nn.get_connection_array(&conns[0]);

		std::map< unsigned int, size_t > input_degree_map;
		for(size_t i = 0; i < num_weights; ++i)
		{
			++input_degree_map[conns[i].to_neuron];
		}

		boost::random::uniform_real_distribution<> rdist(-1.0, 1.0);
		genome gn;
		for(size_t i = 0; i < num_weights; ++i)
		{
			gn.push_back(
				rdist(rgen) / std::sqrt(input_degree_map.at(conns[i].to_neuron))
				);
		}
		return gn;
	}

	static solution decode_genome(genome const& gn)
	{
		size_t const NumWeights = gn.length();
		assert(NumWeights == get_genome_length());
		
		solution sol;
		std::vector< FANN::connection > conns(NumWeights);
		sol.nn.get_connection_array(&conns[0]);
		for(size_t c = 0; c < NumWeights; ++c)
		{
			conns[c].weight = gn[c];
		}
		sol.nn.set_weight_array(&conns[0], NumWeights);

		return sol;
	}
};


#endif


