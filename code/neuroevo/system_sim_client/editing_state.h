// editing_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_EDITING_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_EDITING_STATE_H

#include "clsm/clsm.h"
#include "root_state.h"
#include "ddl/state_machine/paramtree_fsm.h"


namespace sys_control {
	namespace fsm {

		struct editing:
			clsm::clsm_state_base < editing, root >
			, clsm::i_cmd_state
		{
			typedef clsm_state_base base;

			editing(my_context ctx);

			sc::result react(clsm::ev_cmd_str const&);

			typedef mpl::list<
				sc::custom_reaction< clsm::ev_cmd_str >
			> my_reactions;

			typedef mpl::copy<
				my_reactions,
				mpl::inserter< base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			using base::react;

			virtual std::string get_prompt() const override;

			ddl::fsm::paramtree_editor editor;
		};

	}
}


#endif


