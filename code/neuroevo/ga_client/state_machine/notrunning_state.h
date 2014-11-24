// notrunning_state.h

#ifndef __WB_GA_CLIENT_CONTROL_NOTRUNNING_STATE_H
#define __WB_GA_CLIENT_CONTROL_NOTRUNNING_STATE_H

#include "clsm/clsm.h"
#include "active_state.h"

#include "commands/pop_cmd_parser.h"
#include "commands/best_cmd_parser.h"
#include "commands/store_cmd_parser.h"


namespace ga_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct is_completed_choice;

		struct not_running:
			clsm::clsm_state_base< not_running, active, is_completed_choice >
		{
			typedef clsm_state_base base;

			not_running(my_context ctx);

			void on_pop(clsm::ev_cmd< pop_cmd > const& cmd);
			void on_best(clsm::ev_cmd< best_cmd > const& cmd);
			void on_store(clsm::ev_cmd< store_cmd > const& cmd);

			typedef mpl::list<
				sc::in_state_reaction< clsm::ev_cmd< pop_cmd >, not_running, &not_running::on_pop >
				, sc::in_state_reaction< clsm::ev_cmd< best_cmd >, not_running, &not_running::on_best >
				, sc::in_state_reaction< clsm::ev_cmd< store_cmd >, not_running, &not_running::on_store >
			> my_reactions;

			typedef mpl::copy<
				my_reactions,
				mpl::inserter< base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			using base::react;
		};

	}
}


#endif


