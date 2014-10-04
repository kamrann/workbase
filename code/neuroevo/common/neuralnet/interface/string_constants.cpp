// activation_functions.h

#include "neuron_type.h"
#include "activation_functions.h"


namespace nnet {

	const char* const NeuronTypeNames[(int)NeuronType::Count] = {
		"Input",
		"Hidden",
		"Output",
		"Bias",
	};

	const char* const NeuronTypeAbbreviations[(int)NeuronType::Count] = {
		"i",
		"h",
		"o",
		"b",
	};

	const char* const ActivationFnNames[(int)ActivationFnType::Count] = {
		"Sigmoid",
		"Gaussian",
		"Bounded Linear",
		"Threshold",
		"Symmetric Sigmoid",
		"Symmetric Gaussian",
		"Linear",
		"Symmetric Bounded Linear",
		"Symmetric Threshold",
		"Sine",
		"Cosine",
	};

}




