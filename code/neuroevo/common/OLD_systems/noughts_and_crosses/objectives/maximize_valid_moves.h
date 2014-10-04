// maximize_valid_moves.h

#ifndef __NAC_MAXIMIZE_VALID_MOVES_H
#define __NAC_MAXIMIZE_VALID_MOVES_H

#include "shared_objective_fn_data_components.h"

#include "ga/objective_fn.h"


struct max_valid_moves_obj_fn: public objective_fn
{
	typedef boost::mpl::vector< num_moves_ofd, made_invalid_ofd > dependencies;

	typedef double obj_value_t;

	template < typename TrialData, typename ObjFnData >
	static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
	{
		//return obj_value_t(ofd.made_invalid_move ? ((double)(ofd.num_moves - 1) / ofd.num_moves) : 1.0);
		return obj_value_t(ofd.made_invalid_move ? (ofd.num_moves - 1) : ofd.num_moves);
	}
};


#endif


