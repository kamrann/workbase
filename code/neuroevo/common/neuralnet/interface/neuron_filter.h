// neuron_filter.h

#ifndef __WB_NN_NEURON_FILTER_H
#define __WB_NN_NEURON_FILTER_H

#include "neuron_type.h"


namespace nnet {

	class neuron_data;

	class neuron_filter
	{
	public:
		enum class Mode {
			Include,
			Exclude,
		};

	public:
		neuron_filter():
			mode(Mode::Exclude),
			types{}
		{}

		neuron_filter(neuron_type_set const& _types, Mode _mode = Mode::Include):
			mode(_mode),
			types(_types)
		{}

	public:
		bool test(neuron_data const& nr) const;

	public:
		Mode mode;
		neuron_type_set types;
		// TODO:

	public:
		static const neuron_filter Any;
		static const neuron_filter Input;	// Excludes bias
		static const neuron_filter Output;
		static const neuron_filter Hidden;
		static const neuron_filter NonBias;
	};

}


#endif


