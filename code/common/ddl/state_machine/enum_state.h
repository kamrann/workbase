// enum_state.h

#ifndef __WB_PARAMTREE_FSM_ENUM_STATE_H
#define __WB_PARAMTREE_FSM_ENUM_STATE_H

#include "base_node_state.h"

#include "params/enum_add_cmd_parser.h"
#include "params/enum_remove_cmd_parser.h"


namespace ddl {

	namespace fsm {

		struct enumeration:
			clsm::clsm_state_base < enumeration, base_node_state >,
			clsm::i_cmd_state
		{
			typedef clsm_state_base base;

			enumeration(my_context ctx): base(ctx)
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

			void on_add(clsm::ev_cmd< enum_add_cmd > const& cmd);
			void on_remove(clsm::ev_cmd< enum_remove_cmd > const& cmd);

			typedef mpl::list<
				sc::in_state_reaction< clsm::ev_cmd< enum_add_cmd >, enumeration, &enumeration::on_add >
				, sc::in_state_reaction< clsm::ev_cmd< enum_remove_cmd >, enumeration, &enumeration::on_remove >
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


