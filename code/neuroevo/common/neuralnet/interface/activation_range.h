// activation_range.h

#ifndef __WB_NN_ACTIVATION_RANGE_H
#define __WB_NN_ACTIVATION_RANGE_H

//#include "util/multi_interval.h"
#include "basic_types.h"

#include <boost/optional.hpp>

#include <sstream>
#include <limits>
#include <algorithm>


namespace nnet {

/*	namespace bint = boost::numeric;

	typedef multi_interval<
		double,
		bint::interval_lib::policies<
		bint::interval_lib::save_state< bint::interval_lib::rounded_transc_std< double > >,
		bint::interval_lib::checking_base< double >
		>
	> activation_range;
	*/
	typedef range_t activation_range;

}


#endif


