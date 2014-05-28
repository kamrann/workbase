// phys_objective_params.h

#ifndef __NE_PHYS_OBJECTIVE_PARAMS_H
#define __NE_PHYS_OBJECTIVE_PARAMS_H

#include "single_objective_params.h"
#include "pareto_objective_params.h"

#include "rtp_interface/rtp_pop_wide_observer.h"

#include <boost/variant.hpp>


namespace phys {

	typedef boost::variant<
		single_objective_params,
		pareto_objective_params

	> objective_type_specific_params;

	struct phys_objective_params
	{
		i_population_wide_observer::Type	type;
		objective_type_specific_params		type_specific;
	};

}




#endif



