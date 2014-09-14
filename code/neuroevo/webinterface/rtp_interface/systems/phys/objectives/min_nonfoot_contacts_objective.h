// min_nonfoot_contacts_objective.h

#ifndef __RTP_PHYS_MIN_NONFOOT_CONTACTS_OBJECTIVE_H
#define __RTP_PHYS_MIN_NONFOOT_CONTACTS_OBJECTIVE_H

#include "ga/objective_fn.h"
#include "../rtp_phys_agent_body.h"

#include "../bodies/test_biped_body.h"


namespace rtp {

	struct min_nonfoot_contacts_obj_fn: public objective_fn
	{
		typedef boost::mpl::vector<> dependencies;

		typedef double obj_value_t;

		template < typename TrialData, typename ObjFnData >
		static inline obj_value_t evaluate(ObjFnData const& ofd, TrialData const& td)
		{
			return -obj_value_t(((test_biped_body*)td.final_st.body.get())->m_non_foot_contact_count);
		}
	};

}


#endif


