// basic_types.h

#ifndef __WB_NN_BASIC_TYPES_H
#define __WB_NN_BASIC_TYPES_H

#include <vector>


namespace nnet {

	typedef size_t neuron_id;

	typedef double value_t;
	typedef std::vector< value_t > value_array_t;

	typedef value_t weight_t;
	typedef std::vector< weight_t > weight_array_t;

}


#endif


