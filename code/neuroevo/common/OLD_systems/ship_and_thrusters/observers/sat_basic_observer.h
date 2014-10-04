// sat_basic_observer.h

#ifndef __SAT_BASIC_OBSERVER_H
#define __SAT_BASIC_OBSERVER_H


#include "../ship_and_thrusters_system.h"
// TODO: Currently treating observer just like objective fns
// If continue to have identical characteristics, need to rename classes objective_fn, ofdata, etc
#include "../objectives/sat_shared_objective_fn_data_components.h"
#include "ga/objective_fn.h"


struct sat_basic_observer: public objective_fn
{
	typedef boost::mpl::vector<> dependencies;

	struct per_trial_data
	{
		per_trial_data()
		{}
	};

	template < typename TrialData, typename ObjFnData >
	static inline per_trial_data record_observations(ObjFnData const& ofd, TrialData const& td)
	{
		per_trial_data ptd;
		return ptd;
	}
};


#endif


