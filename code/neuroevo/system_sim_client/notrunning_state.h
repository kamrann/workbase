// notrunning_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_NOTRUNNING_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_NOTRUNNING_STATE_H

#include "clsm.h"
#include "active_state.h"


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct is_completed_choice;

		struct not_running:
			clsm::clsm_state_base< not_running, active, is_completed_choice >,
			clsm::i_cmd_state
		{
			typedef mpl::list<
				sc::custom_reaction< ev_get >
			> reactions;

			sc::result react(ev_get const&);
		};

	}
}


#endif


