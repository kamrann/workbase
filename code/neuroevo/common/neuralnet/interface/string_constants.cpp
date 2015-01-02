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

	const char* const ActivationFnNames2[(int)ActivationFnType::Count] = {
		"sigmoid",
		"gaussian",
		"bounded_linear",
		"threshold",
		"symm_sigmoid",
		"symm_gaussian",
		"linear",
		"symm_bounded_linear",
		"symm_threshold",
		"sine",
		"cosine",
	};

	const std::map< std::string, ActivationFnType > ActivationFnNameMap = {
		{ "sigmoid", ActivationFnType::Sigmoid },
		{ "gaussian", ActivationFnType::Gaussian },
		{ "bounded_linear", ActivationFnType::LinearBounded },
		{ "threshold", ActivationFnType::Threshold },
		{ "symm_sigmoid", ActivationFnType::SigmoidSymmetric },
		{ "symm_gaussian", ActivationFnType::GaussianSymmetric },
		{ "linear", ActivationFnType::Linear },
		{ "symm_bounded_linear", ActivationFnType::LinearBoundedSymmetric },
		{ "symm_threshold", ActivationFnType::ThresholdSymmetric },
		{ "sine", ActivationFnType::Sine },
		{ "cosine", ActivationFnType::Cosine },
	};

}




