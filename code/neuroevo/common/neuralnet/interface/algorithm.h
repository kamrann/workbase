// algorithm.h

#ifndef __WB_NN_ALGORITHM_H
#define __WB_NN_ALGORITHM_H

#include "nnet_fwd.h"
#include "activation_functions.h"

#include <map>


namespace nnet {

	struct input_output_ranges
	{
		activation_range input;
		activation_range output;
	};

	typedef std::map< neuron_id, input_output_ranges > input_output_range_map;

	input_output_range_map calculate_input_output_ranges(i_neuralnet const* net, input_range const& input, std::set< neuron_id > const& neurons);
	input_output_range_map calculate_input_output_ranges(i_neuralnet const* net, input_range const& input);
	input_output_ranges calculate_input_output_ranges(i_neuralnet const* net, input_range const& input, neuron_id neuron);

}


#endif


