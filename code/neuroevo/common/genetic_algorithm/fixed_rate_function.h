// fixed_rate_function.h

#ifndef __WB_GA_FIXED_RATE_FUNCTION_H
#define __WB_GA_FIXED_RATE_FUNCTION_H

#include "genalg_common.h"


namespace ga {

	struct rate_context;

	struct fixed_rate_fn
	{
		fixed_rate_fn(double rate):
			_rate(rate)
		{}

		inline double operator() (rate_context const&, rgen_t&) const
		{
			return _rate;
		}

		double const _rate;
	};

}


#endif


