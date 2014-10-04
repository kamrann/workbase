// minimize_linear_speed.h

#ifndef __SAT_MINIMIZE_LINEAR_SPEED_H
#define __SAT_MINIMIZE_LINEAR_SPEED_H

#include "sat_shared_objective_fn_data_components.h"
#include "../ship_and_thrusters_system.h"

#include "ga/objective_fn.h"


struct reduce_lin_speed_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< stopped_moving_linearly_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		// TODO: Should be based on num thrusters, plus thruster power, and also the size of timesteps used in the update function...
//		double const IdealDecelRate = 2 * 0.25;
//		double const SigmoidLimit = 4.0;

		// TODO: .agents[0] ??? need agent index as argument?????
		double end_speed = ofd.stopped_moving ? 0.0 : magnitude(td.final_st.ship.lin_velocity);
		double accel = end_speed - magnitude(td.initial_st.ship.lin_velocity);
		double time = ofd.stopped_moving ? ofd.time_to_stopped_moving : td.final_st.time;// ofd.timesteps;
		double decel_rate = -accel / time;// (timesteps * 0.05);	// TODO: 0.1 copied from system_update_params default value

		// NOTE: If not normalized, could potentially be +ve/-ve and any size
		// Roulette wheel selection requires +ve values with meaningful distribution
//		double sigm = 1.0 / (1.0 + std::exp(-decel_rate * SigmoidLimit / IdealDecelRate));
//		return obj_value_t(sigm);

		return obj_value_t(decel_rate);

//		return obj_value_t(-(magnitude(td.final_st.agents[0].lin_velocity) - magnitude(td.initial_st.agents[0].lin_velocity)));
	}
};

struct min_avg_lin_speed_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< avg_linear_speed_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		return obj_value_t(-ofd.avg_lin_speed);
	}
};



#endif


