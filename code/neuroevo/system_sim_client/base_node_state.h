// base_node_state.h

#ifndef __WB_PARAMTREE_FSM_BASE_NODE_STATE_H
#define __WB_PARAMTREE_FSM_BASE_NODE_STATE_H

#include "paramtree_fsm_defn.h"
#include "clsm.h"

#include "params/param_fwd.h"
#include "params/nav_cmd_parser.h"
#include "params/list_cmd_parser.h"
#include "params/setvalue_cmd_parser.h"
#include "params/save_cmd_parser.h"
#include "params/load_cmd_parser.h"

#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/custom_reaction.hpp>

#include <string>


namespace prm {
	namespace fsm {

		struct general;
		struct enumeration;
		struct random;
		struct repeat;
		struct node_type_choice;

		struct base_node_state:
			clsm::clsm_state_base < base_node_state, paramtree_editor, node_type_choice >
		{
			base_node_state(my_context ctx): clsm_state_base(ctx)
			{
				auto location_type = context< paramtree_editor >().acc.param_here().type;

				auto const is_terminal = is_leaf_type(location_type);
				auto const enable_indexed = !is_terminal;
				auto const require_nav = !is_terminal;

				reg_cmd< nav_cmd >(wrap_grammar< nav_cmd_parser< clsm::iter_t > >(enable_indexed));
				reg_cmd< list_cmd >(wrap_grammar< list_cmd_parser< clsm::iter_t > >(enable_indexed));
				reg_cmd< setvalue_cmd >(wrap_grammar< setvalue_cmd_parser< clsm::iter_t > >(enable_indexed, require_nav));
				reg_cmd< save_cmd >(wrap_grammar< save_cmd_parser< clsm::iter_t > >(enable_indexed));
				reg_cmd< load_cmd >(wrap_grammar< load_cmd_parser< clsm::iter_t > >(enable_indexed));
			}

			qualified_path nav_cmd_path(prm::nav_component const& nv);
			std::string format_value(param val);
			std::string format_type_info(param_tree::param_data const& pd);
			std::string format_constraints(schema::schema_node sch);

			sc::result dispatch(nav_cmd const& cmd);
			sc::result dispatch(list_cmd const& cmd);
			sc::result dispatch(setvalue_cmd const& cmd);
			sc::result dispatch(save_cmd const& cmd);
			sc::result dispatch(load_cmd const& cmd);

//			virtual std::string get_prompt() const override;
		};

	}
}


#endif


