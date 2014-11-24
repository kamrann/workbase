// running_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_RUNNING_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_RUNNING_STATE_H

#include "clsm/clsm.h"
#include "active_state.h"

#include "run_cmd_parser.h"
#include "stop_cmd_parser.h"

#include <boost/optional.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/in_state_reaction.hpp>
#include <boost/mpl/list.hpp>

#include <thread>


namespace sys_control {

	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct not_running;

		struct running:
			clsm::clsm_state_base< running, active >,
			clsm::i_cmd_state
			//	todo:		innermost_state_base< running, active >,
		{
			typedef clsm_state_base base;

			running(my_context ctx);
			~running();

			void on_run(clsm::ev_cmd< run_cmd > const&);

			typedef mpl::list<
				sc::in_state_reaction< clsm::ev_cmd< run_cmd >, running, &running::on_run >
				, sc::transition< clsm::ev_cmd< stop_cmd >, not_running >
			> my_reactions;

			typedef mpl::copy<
				my_reactions,
				mpl::inserter< base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			using base::react;

			void do_run();
			bool interrupt_requested() const;

			virtual std::string get_prompt() const override;

			bool interrupt_request;
			boost::optional< double > run_framerate;
			std::thread run_thread;
		};
	}
}


#endif


