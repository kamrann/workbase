// completed_state.h

#ifndef __WB_GA_CLIENT_CONTROL_COMPLETED_STATE_H
#define __WB_GA_CLIENT_CONTROL_COMPLETED_STATE_H

#include "clsm/clsm.h"
#include "notrunning_state.h"


namespace ga_control {
	namespace fsm {

		struct completed:
			clsm::clsm_state_base< completed, not_running >,
			clsm::i_cmd_state
		{
			typedef clsm_state_base base;

			completed(my_context ctx);

			virtual std::string get_prompt() const override;
		};

	}
}


#endif


