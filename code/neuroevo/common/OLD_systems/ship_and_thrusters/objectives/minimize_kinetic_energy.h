// minimize_kinetic_energy.h

#ifndef __SAT_MINIMIZE_KINETIC_ENERGY_H
#define __SAT_MINIMIZE_KINETIC_ENERGY_H

#include "sat_shared_objective_fn_data_components.h"
#include "../ship_and_thrusters_system.h"

#include "ga/objective_fn.h"


struct min_avg_kinetic_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< avg_linear_speed_ofd, avg_angular_speed_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		// TODO:
		double const m = 1.0;
		double const I = 1.0;

		double const avg_kinetic = 0.5 * (m * ofd.avg_lin_speed * ofd.avg_lin_speed + I * ofd.avg_ang_speed * ofd.avg_ang_speed);
		return obj_value_t(-avg_kinetic);
	}
};

struct reduce_kinetic_obj_fn: public objective_fn
{
	typedef boost::mpl::vector<> dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		// TODO:
		double const m = 1.0;
		double const I = 1.0;

		double const initial_linear = magnitude(td.initial_st.ship.lin_velocity);
		double const initial_angular = magnitude(td.initial_st.ship.ang_velocity);
		double const initial_kinetic = 0.5 * (m * initial_linear * initial_linear + I * initial_angular * initial_angular);
		double const final_linear = magnitude(td.final_st.ship.lin_velocity);
		double const final_angular = magnitude(td.final_st.ship.ang_velocity);
		double const final_kinetic = 0.5 * (m * final_linear * final_linear + I * final_angular * final_angular);
		return obj_value_t(-(final_kinetic - initial_kinetic));
	}
};


#endif


