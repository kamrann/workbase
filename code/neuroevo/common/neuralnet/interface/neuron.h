// neuron.h

#ifndef __WB_NN_NEURON_H
#define __WB_NN_NEURON_H

#include "basic_types.h"
#include "neuron_type.h"
#include "activation_functions.h"


namespace nnet {

	class neuron_data
	{
	public:
		neuron_id id;
		NeuronType type;
		activation_function activation_fn;
	};

/*
	class i_neuron
	{
	public:
		virtual size_t in_connection_count() const = 0;
		virtual size_t out_connection_count() const = 0;
		// TODO: weights, and input/output connection iterators
		virtual ActivationFnType activation_function_type() const = 0;
		// TODO: more info of activation function, which may be specific to the type

	public:
		virtual ~i_neuron()
		{}
	};
*/
}


#endif


