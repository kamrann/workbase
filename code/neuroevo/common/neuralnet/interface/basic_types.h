// basic_types.h

#ifndef __WB_NN_BASIC_TYPES_H
#define __WB_NN_BASIC_TYPES_H

#include "util/multi_interval.h"

#include <vector>


namespace nnet {

	typedef size_t neuron_id;
	typedef size_t connection_id;

	typedef double value_t;
	typedef std::vector< value_t > value_array_t;

	namespace bint = boost::numeric;

	typedef multi_interval <
		double,
		bint::interval_lib::policies <
		bint::interval_lib::save_state< bint::interval_lib::rounded_transc_std< double > >,
		bint::interval_lib::checking_base < double >
		>,
		limited_impl
	>
	range_t;
	typedef std::vector< range_t > range_array_t;

	typedef value_t weight_t;
	typedef std::vector< weight_t > weight_array_t;

}


#endif


