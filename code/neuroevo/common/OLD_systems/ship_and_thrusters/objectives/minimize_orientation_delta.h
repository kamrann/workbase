// minimize_orientation_delta.h

#ifndef __SAT_MINIMIZE_ORIENTATION_DELTA_H
#define __SAT_MINIMIZE_ORIENTATION_DELTA_H

#include "sat_shared_objective_fn_data_components.h"
#include "../ship_and_thrusters_system.h"

#include "ga/objective_fn.h"


struct min_orient_delta_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< avg_orientation_delta_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		return obj_value_t(boost::math::double_constants::pi - ofd.avg_delta);
	}
};


#endif


