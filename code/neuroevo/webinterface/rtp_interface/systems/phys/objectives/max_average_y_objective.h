// max_average_y_objective.h

#ifndef __RTP_PHYS_MAX_AVERAGE_Y_OBJECTIVE_H
#define __RTP_PHYS_MAX_AVERAGE_Y_OBJECTIVE_H

#include "shared_components.h"
#include "ga/objective_fn.h"

#include <boost/math/constants/constants.hpp>


namespace rtp {

	struct max_average_y_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector< avg_height_ofd > dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			return obj_value_t(ofd.avg_height);
		}
	};

}


#endif


