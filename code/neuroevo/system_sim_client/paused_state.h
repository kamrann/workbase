// paused_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_PAUSED_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_PAUSED_STATE_H

#include "clsm.h"
#include "notrunning_state.h"


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct paused:
			clsm::clsm_state_base< paused, not_running >,
			clsm::i_cmd_state
		{
			paused(my_context ctx);

			typedef mpl::list<
				sc::custom_reaction< ev_step >
				, sc::custom_reaction< ev_run >
			> my_reactions;

/*			typedef mpl::copy<
				my_reactions,
				mpl::inserter< innermost_state_base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;
*/
			virtual std::string get_prompt() const override;

			sc::result react(ev_step const&);
			sc::result react(ev_run const&);
		};

	}
}


#endif


