// minimize_opposing_thrusters.h

#ifndef __SAT_MINIMIZE_OPPOSING_THRUSTERS_H
#define __SAT_MINIMIZE_OPPOSING_THRUSTERS_H

#include "sat_shared_objective_fn_data_components.h"
#include "../ship_and_thrusters_system.h"

#include "ga/objective_fn.h"


struct min_opp_thrusters_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< opposing_thruster_activation_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		return obj_value_t(1.0 - ofd.opp_thrusters_freq);
	}
};


#endif


