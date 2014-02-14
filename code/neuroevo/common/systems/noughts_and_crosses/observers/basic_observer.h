// basic_observer.h

#ifndef __NAC_BASIC_OBSERVER_H
#define __NAC_BASIC_OBSERVER_H


#include "../noughts_and_crosses_system.h"
// TODO: Currently treating observer just like objective fns
// If continue to have identical characteristics, need to rename classes objective_fn, ofdata, etc
#include "../objectives/shared_objective_fn_data_components.h"
#include "ga/objective_fn.h"


struct basic_observer: public objective_fn
{
	typedef boost::mpl::vector<> dependencies;

	struct per_trial_data
	{
		size_t total_moves_made;	// By either player
		boost::optional< noughts_and_crosses_base::Player > winner;
		bool game_completed;

		per_trial_data(): total_moves_made(0), winner(), game_completed(false)
		{}
	};

	template < typename TrialData, typename ObjFnData >
	static inline per_trial_data record_observations(ObjFnData const& ofd, TrialData const& td)
	{
		per_trial_data ptd;
		ptd.total_moves_made = td.final_st.num_played_squares();
		ptd.winner = td.final_st.is_winner();
		ptd.game_completed = ptd.winner || td.final_st.num_unplayed_squares() == 0;
		return ptd;
	}
};


#endif


