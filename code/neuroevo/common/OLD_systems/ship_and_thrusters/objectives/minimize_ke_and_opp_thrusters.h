// minimize_ke_and_opp_thrusters.h

#ifndef __SAT_MINIMIZE_KE_AND_OPPOSING_THRUSTERS_H
#define __SAT_MINIMIZE_KE_AND_OPPOSING_THRUSTERS_H

#include "sat_shared_objective_fn_data_components.h"
#include "../ship_and_thrusters_system.h"

#include "ga/objective_fn.h"


struct min_ke_and_opp_thrusters_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< avg_linear_speed_ofd, avg_angular_speed_ofd, opposing_thruster_activation_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		double const avg_kinetic = ofd.avg_lin_speed * ofd.avg_lin_speed + ofd.avg_ang_speed * ofd.avg_ang_speed;
		return obj_value_t((1.0 - ofd.opp_thrusters_freq) / (1.0 + avg_kinetic));
	}
};


#endif


