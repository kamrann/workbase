// repeat_state.h

#ifndef __WB_PARAMTREE_FSM_REPEAT_STATE_H
#define __WB_PARAMTREE_FSM_REPEAT_STATE_H

#include "base_node_state.h"

#include "../commands/repeat_add_cmd_parser.h"
#include "../commands/repeat_remove_cmd_parser.h"


namespace ddl {
	namespace fsm {

		struct repeat:
			clsm::clsm_state_base < repeat, base_node_state >,
			clsm::i_cmd_state
		{
			typedef clsm_state_base base;

			repeat(my_context ctx): base(ctx)
			{
				reg_cmd< repeat_add_cmd >(wrap_grammar< repeat_add_cmd_parser< clsm::iter_t > >());
				reg_cmd< repeat_remove_cmd >(wrap_grammar< repeat_remove_cmd_parser< clsm::iter_t > >());
			}

			void on_add(clsm::ev_cmd< repeat_add_cmd > const& cmd);
			void on_remove(clsm::ev_cmd< repeat_remove_cmd > const& cmd);

			typedef mpl::list<
				sc::in_state_reaction< clsm::ev_cmd< repeat_add_cmd >, repeat, &repeat::on_add >
				, sc::in_state_reaction< clsm::ev_cmd< repeat_remove_cmd >, repeat, &repeat::on_remove >
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


