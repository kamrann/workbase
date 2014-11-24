// notready_state.h

#ifndef __WB_GA_CLIENT_CONTROL_NOT_READY_STATE_H
#define __WB_GA_CLIENT_CONTROL_NOT_READY_STATE_H

#include "clsm/clsm.h"
#include "inactive_state.h"


namespace ga_control {
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


