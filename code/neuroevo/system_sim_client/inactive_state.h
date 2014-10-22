// inactive_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_INACTIVE_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_INACTIVE_STATE_H

#include "clsm.h"
#include "root_state.h"


namespace sys_control {
	namespace fsm {

		struct is_ready_choice;

		struct inactive:
			clsm::clsm_state_base < inactive, root, is_ready_choice >,
			clsm::i_cmd_state
		{
			using clsm_state_base::clsm_state_base;
		};

	}
}


#endif


