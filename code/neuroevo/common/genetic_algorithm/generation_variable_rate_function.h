// generation_variable_rate_function.h

#ifndef __WB_GA_GENERATION_VARIABLE_RATE_FUNCTION_H
#define __WB_GA_GENERATION_VARIABLE_RATE_FUNCTION_H

#include "rate_function.h"
#include "interpolated_function.h"


namespace ga {

	struct generation_variable_rate_fn:
		public interpolated_function< double, size_t >
	{
		typedef interpolated_function base_t;

		generation_variable_rate_fn(double initial_rate, control_point cutoff):
			base_t(initial_rate, cutoff)
		{}

		inline double operator() (rate_context const& ctx, rgen_t&) const
		{
			return base_t::operator() (ctx.generation);
		}
	};

}


#endif


