// enum_state.h

#ifndef __WB_PARAMTREE_FSM_ENUM_STATE_H
#define __WB_PARAMTREE_FSM_ENUM_STATE_H

#include "base_node_state.h"

#include "params/enum_add_cmd_parser.h"
#include "params/enum_remove_cmd_parser.h"


namespace prm {

	namespace fsm {

		struct enumeration:
			clsm::clsm_state_base < enumeration, base_node_state >,
			clsm::i_cmd_state
		{
			enumeration(my_context ctx): clsm_state_base(ctx)
			{
				auto const& editor = context< paramtree_editor >();
				auto path = editor.acc.where();
				auto acc = editor.acc;	// TODO: think schema_provider sig should change, surely doesn't need non-const ref to accessor?
				auto sch = (*editor.provider)[path.unindexed()](acc);

				// Only allow add/remove if multi-select enum
				auto maxsel = sch["constraints"]["maxsel"].as< int >();
				if(maxsel > 2)
				{
					reg_cmd< enum_add_cmd >(wrap_grammar< enum_add_cmd_parser< clsm::iter_t > >());
					reg_cmd< enum_remove_cmd >(wrap_grammar< enum_remove_cmd_parser< clsm::iter_t > >());
				}
			}

			sc::result dispatch(enum_add_cmd const& cmd);
			sc::result dispatch(enum_remove_cmd const& cmd);

			virtual std::string get_prompt() const override;
		};

	}
}


#endif


