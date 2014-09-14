// layers.h

#ifndef __WB_NN_LAYERS_H
#define __WB_NN_LAYERS_H

#include <set>


namespace nnet {

	enum class LayerType {
		Input,
		Hidden,
		Output,
	};

	typedef std::set< LayerType > layer_type_set;

}


#endif


