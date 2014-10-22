// ready_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_READY_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_READY_STATE_H

#include "clsm.h"
#include "inactive_state.h"


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct ready:
			clsm::clsm_state_base < ready, inactive >,
			clsm::i_cmd_state
		{
			ready(my_context ctx);

			typedef mpl::list<
				sc::transition< ev_init, active >
			> my_reactions;

/*			typedef mpl::copy<
				my_reactions,
				mpl::inserter< innermost_state_base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;
*/
			virtual std::string get_prompt() const override;
		};

	}
}


#endif


