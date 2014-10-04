// rtp_mlp_controller.cpp

#include "rtp_mlp_controller.h"

#include <cassert>


namespace rtp {

	void generic_mlp_controller::set_weights(std::vector< double > const& weights)
	{
/*		size_t const NumWeights = weights.size();
		size_t const NumConnections = nn.get_total_connections();
		assert(NumWeights == NumConnections);

		std::vector< FANN::connection > conns(NumWeights);
		nn.get_connection_array(&conns[0]);
		for(size_t c = 0; c < NumWeights; ++c)
		{
			conns[c].weight = weights[c];
		}
		nn.set_weight_array(&conns[0], NumWeights);
		*/

		size_t const NumWeights = weights.size();
		size_t const NumConnections = nn.num_connections();
		assert(NumWeights == NumConnections);
		
		nn.load_weights(weights);
	}

}

