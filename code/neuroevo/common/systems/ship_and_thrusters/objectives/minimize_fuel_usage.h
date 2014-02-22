// minimize_fuel_usage.h

#ifndef __SAT_MINIMIZE_FUEL_USAGE_H
#define __SAT_MINIMIZE_FUEL_USAGE_H

#include "sat_shared_objective_fn_data_components.h"
#include "../ship_and_thrusters_system.h"

#include "ga/objective_fn.h"


struct min_fuel_obj_fn: public objective_fn
{
	typedef boost::mpl::vector<> dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		// TODO:
		return obj_value_t(0);
	}
};


#endif


