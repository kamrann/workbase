// max_final_x_objective.h

#ifndef __RTP_PHYS_MAX_FINAL_X_OBJECTIVE_H
#define __RTP_PHYS_MAX_FINAL_X_OBJECTIVE_H

#include "ga/objective_fn.h"
#include "../rtp_phys_agent_body.h"


namespace rtp {

	struct max_final_x_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector<> dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			return obj_value_t(td.final_st.body->get_position().x);
		}
	};

}


#endif


