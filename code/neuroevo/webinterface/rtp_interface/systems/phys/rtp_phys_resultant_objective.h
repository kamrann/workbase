// rtp_phys_resultant_objective.h

#ifndef __NE_RTP_PHYS_RESULTANT_OBJECTIVE_H
#define __NE_RTP_PHYS_RESULTANT_OBJECTIVE_H

#include "rtp_phys_objectives.h"
#include "../../rtp_pop_wide_observer.h"


namespace rtp_phys
{
	class resultant_objective
	{
	public:
		static i_population_wide_observer* create_instance(i_population_wide_observer::Type type, rtp_param param, std::set< agent_objective::Type >& required_observations);
	};
}


#endif

