// try_to_win.h

#ifndef __NAC_TRY_TO_WIN_H
#define __NAC_TRY_TO_WIN_H

#include "shared_objective_fn_data_components.h"
#include "../noughts_and_crosses_system.h"

#include "ga/objective_fn.h"


struct try_to_win_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< num_moves_ofd, made_invalid_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		auto is_winner = td.final_st.is_winner();
		if(is_winner)
		{
			// Won or lost
			return *is_winner == noughts_and_crosses_base::Player::Crosses ? 5.0 : 0.0;
		}
		else if(td.final_st.num_unplayed_squares() == 0)
		{
			// Tied
			return 1.0;
		}
		else
		{
			// Must have made an invalid move
			return -10.0 + 5.0 * ((double)(ofd.num_moves - 1) / ofd.num_moves);
		}
	}
};


#endif


