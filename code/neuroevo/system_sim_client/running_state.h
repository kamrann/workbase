// running_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_RUNNING_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_RUNNING_STATE_H

#include "clsm.h"
#include "active_state.h"

#include <boost/optional.hpp>
//#include <boost/statechart/transition.hpp>
//#include <boost/statechart/custom_reaction.hpp>
#include <boost/mpl/list.hpp>

#include <thread>


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct running:
			clsm::clsm_state_base< running, active >,
			clsm::i_cmd_state
			//	todo:		innermost_state_base< running, active >,
		{
			running(my_context ctx);

			typedef mpl::list<
				sc::custom_reaction< ev_run >
				, sc::transition< ev_pause, not_running >
			> my_reactions;

			typedef mpl::copy<
				my_reactions,
				mpl::inserter< innermost_state_base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			~running();

			void do_run();
			bool interrupt_requested() const;

			virtual std::string get_prompt() const override;

			sc::result react(ev_run const&);

			bool interrupt_request;
			boost::optional< double > run_framerate;
			std::thread run_thread;
		};
	}
}


#endif


