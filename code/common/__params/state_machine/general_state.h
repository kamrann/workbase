// general_state.h

#ifndef __WB_PARAMTREE_FSM_GENERAL_STATE_H
#define __WB_PARAMTREE_FSM_GENERAL_STATE_H

#include "base_node_state.h"

#include "params/qualified_path.h"


namespace prm {
	namespace fsm {

		struct general:
			clsm::clsm_state_base < general, base_node_state >,
			clsm::i_cmd_state
		{
			using clsm_state_base::clsm_state_base;

			virtual std::string get_prompt() const override
			{
				auto location = context< paramtree_editor >().acc.where();
				return location.to_string() + ": ";
			}
		};

	}
}


#endif


