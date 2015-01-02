// modifiable.h

#ifndef __WB_NN_MODIFIABLE_H
#define __WB_NN_MODIFIABLE_H

#include "basic_types.h"


namespace nnet {

	struct activation_function;
	//struct layer_data;

	// TODO: Separate (or derived, since structure modifiable implies modifiable?) i_structure_modifiable interface
	class i_modifiable
	{
	public:
// TODO: probably don't want per neuron		virtual void set_activation_fn(neuron_id neuron, activation_function fn) = 0;
		virtual void set_weight(connection_id conn, value_t weight) = 0;

	public:
		virtual ~i_modifiable()
		{}
	};

}


#endif


