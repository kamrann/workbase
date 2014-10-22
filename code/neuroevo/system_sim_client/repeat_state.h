// repeat_state.h

#ifndef __WB_PARAMTREE_FSM_REPEAT_STATE_H
#define __WB_PARAMTREE_FSM_REPEAT_STATE_H

#include "base_node_state.h"

#include "params/repeat_add_cmd_parser.h"
#include "params/repeat_remove_cmd_parser.h"


namespace prm {
	namespace fsm {

		struct repeat:
			clsm::clsm_state_base < repeat, base_node_state >,
			clsm::i_cmd_state
		{
			repeat(my_context ctx): clsm_state_base(ctx)
			{
				reg_cmd< repeat_add_cmd >(wrap_grammar< repeat_add_cmd_parser< clsm::iter_t > >());
				reg_cmd< repeat_remove_cmd >(wrap_grammar< repeat_remove_cmd_parser< clsm::iter_t > >());
			}

			sc::result dispatch(repeat_add_cmd const& cmd);
			sc::result dispatch(repeat_remove_cmd const& cmd);

			virtual std::string get_prompt() const override;
		};

	}
}


#endif


