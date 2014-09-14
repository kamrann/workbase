// maintain_upright_stance_objective.h

#ifndef __RTP_PHYS_MAINTAIN_UPRIGHT_STANCE_OBJECTIVE_H
#define __RTP_PHYS_MAINTAIN_UPRIGHT_STANCE_OBJECTIVE_H

#include "shared_components.h"
#include "ga/objective_fn.h"

#include <boost/math/constants/constants.hpp>


namespace rtp {

/*	struct maintain_upright_stance_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector< avg_tilt_ofd > dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			return obj_value_t(boost::math::double_constants::pi - ofd.avg_tilt);
		}
	};
*/
}


#endif


