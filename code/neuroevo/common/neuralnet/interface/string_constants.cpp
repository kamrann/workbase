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

	const char* const ActivationFnNames[(int)ActivationFn::Count] = {
		"Sigmoid",
		"Gaussian",
		"Threshold",
		"Linear",
		"Bounded Linear",
		"Symmetric Sigmoid",
		"Symmetric Gaussian",
		"Symmetric Threshold",
		"Sine",
		"Cosine",
	};

}




