// notrunning_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_NOTRUNNING_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_NOTRUNNING_STATE_H

#include "clsm/clsm.h"
#include "active_state.h"

#include "get_cmd_parser.h"
#include "drawer_cmd_parser.h"


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct is_completed_choice;

		struct not_running:
			clsm::clsm_state_base< not_running, active, is_completed_choice >
		{
			typedef clsm_state_base base;

			not_running(my_context ctx): base(ctx)
			{
				reg_cmd< get_cmd >(wrap_grammar< get_cmd_parser< clsm::iter_t > >());
				reg_cmd< drawer_cmd >(wrap_grammar< drawer_cmd_parser< clsm::iter_t > >());
			}

			void on_get(clsm::ev_cmd< get_cmd > const& cmd)
			{
				// Send the requested system state values to the output sink
				auto val_ids = sys::state_value_id_list{};
				for(auto const& str : cmd.values)
				{
					val_ids.push_back(sys::state_value_id::from_string(str));
				}
				context< active >().output_state_values(val_ids);
			}

			typedef mpl::list<
				sc::in_state_reaction< clsm::ev_cmd< get_cmd >, not_running, &not_running::on_get >
				, sc::in_state_reaction< clsm::ev_cmd< drawer_cmd >, active, &active::on_drawer >
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


