// single_objective_params.h

#ifndef __NE_PARETO_OBJECTIVE_PARAMS_H
#define __NE_PARETO_OBJECTIVE_PARAMS_H

#include "rtp_interface/systems/phys/rtp_phys_objectives.h"

#include <set>


namespace phys {

	struct pareto_objective_params
	{
		std::set< rtp_phys::agent_objective::Type >		objectives;
	};

}




#endif



