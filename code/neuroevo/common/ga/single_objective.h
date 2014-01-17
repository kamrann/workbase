// single_objective.h

#ifndef __SINGLE_OBJECTIVE_H
#define __SINGLE_OBJECTIVE_H

#include "util/meta_dependencies.h"
#include "objective_fn.h"


template <
	typename TrialData,
	typename ObjFunction		// Type deriving from objective_fn
>
class single_objective
{
private:
	typedef ObjFunction obj_fn_t;
	typedef typename meta_util::unique_dependencies< obj_fn_t >::type ofd_components_tl;

public:
	typedef typename obj_fn_t::obj_value_t obj_value_t;
	typedef ofdata< ofd_components_tl > ofdata_t;

public:
	inline obj_value_t evaluate(ofdata_t const& ofd, TrialData const& td) const
	{
		return obj_fn_t::evaluate(ofd, td);
	}
};


#endif


