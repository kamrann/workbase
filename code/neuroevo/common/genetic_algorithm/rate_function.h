// rate_function.h

#ifndef __WB_GA_RATE_FUNCTION_H
#define __WB_GA_RATE_FUNCTION_H

#include "genalg_common.h"

#include <functional>


namespace ga {

	struct rate_context
	{
		size_t generation;

		// TODO: include rgen here?
	};

	typedef std::function< double(rate_context const&, rgen_t&) > rate_fn_t;

}


#endif


