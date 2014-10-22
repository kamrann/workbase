// completed_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_COMPLETED_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_COMPLETED_STATE_H

#include "clsm.h"
#include "notrunning_state.h"


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct completed:
			clsm::clsm_state_base< completed, not_running >,
			clsm::i_cmd_state
		{
			completed(my_context ctx);

			virtual std::string get_prompt() const override;
		};

	}
}


#endif


