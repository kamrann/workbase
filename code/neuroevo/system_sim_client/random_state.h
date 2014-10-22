// random_state.h

#ifndef __WB_PARAMTREE_FSM_RANDOM_STATE_H
#define __WB_PARAMTREE_FSM_RANDOM_STATE_H

#include "base_node_state.h"


namespace prm {

	namespace fsm {

		struct random:
			clsm::clsm_state_base < random, base_node_state >,
			clsm::i_cmd_state
		{
			random(my_context ctx): clsm_state_base(ctx)
			{
				reg_cmd< prm::random >(wrap_grammar_prefixed< random_parser< clsm::iter_t >::lax >("set"));
			}

			sc::result dispatch(prm::random const& cmd);

			virtual std::string get_prompt() const override;
		};

	}
}


#endif


