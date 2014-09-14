// layer_access.h

#ifndef __WB_NN_LAYER_ACCESS_H
#define __WB_NN_LAYER_ACCESS_H

#include "basic_types.h"

#include <memory>


namespace nnet {

	typedef size_t layer_idx_t;

	class i_neuron_access;

	class i_layers
	{
	public:
		virtual size_t layer_count() const = 0;
		virtual size_t hidden_layer_count() const = 0;
		virtual size_t layer_size(layer_idx_t layer) const = 0;

		virtual std::unique_ptr< i_neuron_access > get_layer(layer_idx_t layer) = 0;

	public:
		virtual ~i_layers()
		{}
	};

}


#endif


