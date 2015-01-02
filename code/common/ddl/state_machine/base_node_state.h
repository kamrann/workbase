// base_node_state.h

#ifndef __WB_PARAMTREE_FSM_BASE_NODE_STATE_H
#define __WB_PARAMTREE_FSM_BASE_NODE_STATE_H

#include "paramtree_fsm_defn.h"
#include "clsm/clsm.h"

#include "../commands/nav_cmd_parser.h"
#include "../commands/list_cmd_parser.h"
#include "../commands/setvalue_cmd_parser.h"
#include "../commands/reset_cmd_parser.h"
#include "../commands/save_cmd_parser.h"
#include "../commands/load_cmd_parser.h"

#include "../values/value.h"

#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/in_state_reaction.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/termination.hpp>

#include <string>


namespace ddl {
	namespace fsm {

		struct node_type_choice;

		struct quit_cmd{};

		struct base_node_state:
			clsm::clsm_state_base < base_node_state, paramtree_editor, node_type_choice >
		{
			typedef clsm_state_base base;

			base_node_state(my_context ctx);

			bool at_leaf_node() const;
			sch_node schema_node_at(path const&) const;
			sch_node current_schema_node() const;

			navigator nav_to_node(nav_component const& nv);
			std::string format_value(value val);
			std::string format_type_info(sch_node const& sch);
			std::string format_constraints(sch_node const& sch);

			sc::result react(clsm::ev_cmd< nav_cmd > const& cmd);
			void on_list(clsm::ev_cmd< list_cmd > const& cmd);
			void on_setvalue(clsm::ev_cmd< setvalue_cmd > const& cmd);
			void on_reset(clsm::ev_cmd< reset_cmd > const& cmd);
			void on_save(clsm::ev_cmd< save_cmd > const& cmd);
			void on_load(clsm::ev_cmd< load_cmd > const& cmd);

			typedef mpl::list<
				sc::custom_reaction< clsm::ev_cmd< nav_cmd > >
				, sc::in_state_reaction< clsm::ev_cmd< list_cmd >, base_node_state, &base_node_state::on_list >
				, sc::in_state_reaction< clsm::ev_cmd< setvalue_cmd >, base_node_state, &base_node_state::on_setvalue >
				, sc::in_state_reaction< clsm::ev_cmd< reset_cmd >, base_node_state, &base_node_state::on_reset >
				, sc::in_state_reaction< clsm::ev_cmd< save_cmd >, base_node_state, &base_node_state::on_save >
				, sc::in_state_reaction< clsm::ev_cmd< load_cmd >, base_node_state, &base_node_state::on_load >
				, sc::termination< clsm::ev_cmd< quit_cmd > >
			> my_reactions;

			/*
			TODO:
			Can probably avoid requiring all clsm_state_base-derived states to duplicate the following by
			allowing the above my_reactions list to be specified as a template parameter to clsm_state_base.
			Then the derived class just implements the reaction handler methods.
			*/
			typedef mpl::copy<
				my_reactions,
				mpl::inserter< base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			using base::react;

//			virtual std::string get_prompt() const override;
		};

	}
}


#endif


