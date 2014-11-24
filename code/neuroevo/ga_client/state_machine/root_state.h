// root_state.h

#ifndef __WB_GA_CLIENT_CONTROL_ROOT_STATE_H
#define __WB_GA_CLIENT_CONTROL_ROOT_STATE_H

#include "clsm/clsm.h"
#include "control_fsm_defn.h"


namespace ga_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct inactive;

		struct root:
			clsm::clsm_state_base < root, ga_controller, inactive >
		{
			typedef clsm_state_base base;

			using base::base;

// todo:			typedef sc::termination< ev_quit > reactions;
		};

	}
}


#endif


