// min_average_ke_objective.h

#ifndef __RTP_PHYS_MIN_AVERAGE_KE_OBJECTIVE_H
#define __RTP_PHYS_MIN_AVERAGE_KE_OBJECTIVE_H

#include "shared_components.h"
#include "ga/objective_fn.h"


namespace rtp_phys {

	struct min_average_ke_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector< avg_ke_ofd > dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			return obj_value_t(-ofd.avg_ke);
		}
	};

}


#endif


