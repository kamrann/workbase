// layer_filter.h

#ifndef __WB_NN_LAYER_FILTER_H
#define __WB_NN_LAYER_FILTER_H

#include "layers.h"


namespace nnet {

	class layer_filter
	{
	public:
		enum class Mode {
			Include,
			Exclude,
		};

	public:
		layer_filter():
			mode(Mode::Exclude),
			types{}
		{}

		layer_filter(layer_set const& _types, Mode _mode = Mode::Include):
			mode(_mode),
			types(_types)
		{}

	public:
		bool test(layer_data const& ly) const;

	public:
		Mode mode;
		layer_set types;

	public:
		static const layer_filter Any;
		static const layer_filter Input;
		static const layer_filter Output;
		static const layer_filter Hidden;
	};

}


#endif


