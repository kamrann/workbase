// maintain_altitude_objective.h

#ifndef __RTP_PHYS_MAINTAIN_ALTITUDE_OBJECTIVE_H
#define __RTP_PHYS_MAINTAIN_ALTITUDE_OBJECTIVE_H

#include "shared_components.h"
#include "ga/objective_fn.h"


namespace rtp {

	struct maintain_altitude_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector< avg_y_delta_ofd< 10 > > dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			return obj_value_t(-ofd.avg_y_delta);
		}
	};

}


#endif


