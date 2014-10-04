// neuron_type.h

#ifndef __WB_NN_NEURON_TYPE_H
#define __WB_NN_NEURON_TYPE_H

#include <set>


namespace nnet {

	enum class NeuronType {
		Input,
		Hidden,
		Output,
		Bias,

		Count,
	};

	extern const char* const NeuronTypeNames[(int)NeuronType::Count];
	extern const char* const NeuronTypeAbbreviations[(int)NeuronType::Count];

	typedef std::set< NeuronType > neuron_type_set;

	inline bool has_inputs(NeuronType type)
	{
		return type == NeuronType::Hidden || type == NeuronType::Output;
	}

}


#endif


