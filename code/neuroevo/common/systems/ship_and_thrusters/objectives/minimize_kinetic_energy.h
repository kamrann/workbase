// minimize_kinetic_energy.h

#ifndef __SAT_MINIMIZE_KINETIC_ENERGY_H
#define __SAT_MINIMIZE_KINETIC_ENERGY_H

#include "sat_shared_objective_fn_data_components.h"
#include "../ship_and_thrusters_system.h"

#include "ga/objective_fn.h"


struct min_kinetic_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< avg_linear_speed_ofd, avg_angular_speed_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
/*		double const initial_linear = magnitude(td.initial_st.agents[0].lin_velocity);
		double const initial_angular = magnitude(td.initial_st.agents[0].ang_velocity);
		double const initial_kinetic = initial_linear * initial_linear + initial_angular * initial_angular;
		double const final_linear = magnitude(td.final_st.agents[0].lin_velocity);
		double const final_angular = magnitude(td.final_st.agents[0].ang_velocity);
		double const final_kinetic = final_linear * final_linear + final_angular * final_angular;
		return obj_value_t(-(final_kinetic - initial_kinetic));
*/
		double const avg_kinetic = ofd.avg_lin_speed * ofd.avg_lin_speed + ofd.avg_ang_speed * ofd.avg_ang_speed;
		return obj_value_t(-avg_kinetic);
	}
};


#endif


