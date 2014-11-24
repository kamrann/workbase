// paused_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_PAUSED_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_PAUSED_STATE_H

#include "clsm/clsm.h"
#include "notrunning_state.h"

#include "run_cmd_parser.h"
#include "step_cmd_parser.h"
#include "chart_cmd_parser.h"


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct completed;

		struct paused:
			clsm::clsm_state_base< paused, not_running >,
			clsm::i_cmd_state
		{
			typedef clsm_state_base base;

			paused(my_context ctx): base(ctx)
			{
				reg_cmd< step_cmd >(wrap_grammar< step_cmd_parser< clsm::iter_t > >());
				reg_cmd< run_cmd >(wrap_grammar< run_cmd_parser< clsm::iter_t > >());
				reg_cmd< chart_cmd >(wrap_grammar< chart_cmd_parser< clsm::iter_t > >());
			}

			sc::result paused::react(clsm::ev_cmd< step_cmd > const& cmd);
			sc::result paused::react(clsm::ev_cmd< run_cmd > const& cmd);

			typedef mpl::list<
				sc::custom_reaction< clsm::ev_cmd< step_cmd > >
				, sc::custom_reaction< clsm::ev_cmd< run_cmd > >
				, sc::in_state_reaction< clsm::ev_cmd< chart_cmd >, active, &active::on_chart >
			> my_reactions;

			typedef mpl::copy<
				my_reactions,
				mpl::inserter< base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			using base::react;

			virtual std::string get_prompt() const override;
		};

	}
}


#endif


