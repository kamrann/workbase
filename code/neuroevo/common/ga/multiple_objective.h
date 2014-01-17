// multiple_objective.h

#ifndef __MULTIPLE_OBJECTIVE_H
#define __MULTIPLE_OBJECTIVE_H

#include "objective_fn.h"


template <
	typename TrialData,
	typename ObjFunctions		// MPL list of types deriving from objective_fn
>
class multiple_objective
{
private:
	typedef ObjFunctions obj_fns_tl;
	typedef typename meta_util::unique_dependencies< obj_fns_tl >::type ofd_components_tl;

public:
	typedef ofdata< ofd_components_tl > ofdata_t;
};


#endif


