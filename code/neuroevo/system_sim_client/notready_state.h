// notready_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_NOT_READY_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_NOT_READY_STATE_H

#include "clsm.h"
#include "inactive_state.h"


namespace sys_control {
	namespace fsm {

		struct not_ready:
			clsm::clsm_state_base < not_ready, inactive >, 
			clsm::i_cmd_state
		{
			using clsm_state_base::clsm_state_base;

			virtual std::string get_prompt() const override;
		};

	}
}


#endif


