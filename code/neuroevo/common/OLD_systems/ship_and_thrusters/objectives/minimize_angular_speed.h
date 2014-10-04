// minimize_angular_speed.h

#ifndef __SAT_MINIMIZE_ANGULAR_SPEED_H
#define __SAT_MINIMIZE_ANGULAR_SPEED_H

#include "sat_shared_objective_fn_data_components.h"
#include "../ship_and_thrusters_system.h"

#include "ga/objective_fn.h"


struct reduce_ang_speed_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< stopped_rotating_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		// TODO: as in above method
//		double const IdealDecelRate = 2 * 0.25;
//		double const SigmoidLimit = 4.0;

		double end_speed = ofd.stopped_rotating ? 0.0 : magnitude(td.final_st.ship.ang_velocity);
		double accel = end_speed - magnitude(td.initial_st.ship.ang_velocity);
		double time = ofd.stopped_rotating ? ofd.time_to_stopped_rotating : td.final_st.time;//ofd.timesteps;
		double decel_rate = -accel / time;//(timesteps * 0.05);

		// NOTE: If not normalized, could potentially be +ve/-ve and any size
		// Roulette wheel selection requires +ve values with meaningful distribution
//		double sigm = 1.0 / (1.0 + std::exp(-decel_rate * SigmoidLimit / IdealDecelRate));
//		return obj_value_t(sigm);

		return obj_value_t(decel_rate);

//		return obj_value_t(-(magnitude(td.final_st.agents[0].ang_velocity) - magnitude(td.initial_st.agents[0].ang_velocity)));
	}
};

struct min_avg_ang_speed_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< avg_angular_speed_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		return obj_value_t(-ofd.avg_ang_speed);
	}
};


#endif


