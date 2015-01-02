// base_node_state.h

#ifndef __WB_PARAMTREE_FSM_BASE_NODE_STATE_H
#define __WB_PARAMTREE_FSM_BASE_NODE_STATE_H

#include "paramtree_fsm_defn.h"
#include "clsm/clsm.h"

#include "params/param_fwd.h"
#include "params/nav_cmd_parser.h"
#include "params/list_cmd_parser.h"
#include "params/setvalue_cmd_parser.h"
#include "params/save_cmd_parser.h"
#include "params/load_cmd_parser.h"

#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/in_state_reaction.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/termination.hpp>

#include <string>


namespace prm {
	namespace fsm {

		struct node_type_choice;

		struct quit_cmd{};

		struct base_node_state:
			clsm::clsm_state_base < base_node_state, paramtree_editor, node_type_choice >
		{
			typedef clsm_state_base base;

			base_node_state(my_context ctx): base(ctx)
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

				auto quit_str = context< paramtree_editor >().quit_str;
				reg_cmd< quit_cmd >([quit_str]()->qi::rule< clsm::iter_t, quit_cmd(), clsm::skip_t >{ return qi::lit(quit_str)[phx::nothing]; });
			}

			qualified_path nav_cmd_path(prm::nav_component const& nv);
			std::string format_value(param val);
			std::string format_type_info(param_tree::param_data const& pd);
			std::string format_constraints(schema::schema_node sch);

			sc::result react(clsm::ev_cmd< nav_cmd > const& cmd);
			void on_list(clsm::ev_cmd< list_cmd > const& cmd);
			void on_setvalue(clsm::ev_cmd< setvalue_cmd > const& cmd);
			void on_save(clsm::ev_cmd< save_cmd > const& cmd);
			void on_load(clsm::ev_cmd< load_cmd > const& cmd);

			typedef mpl::list<
				sc::custom_reaction< clsm::ev_cmd< nav_cmd > >
				, sc::in_state_reaction< clsm::ev_cmd< list_cmd >, base_node_state, &base_node_state::on_list >
				, sc::in_state_reaction< clsm::ev_cmd< setvalue_cmd >, base_node_state, &base_node_state::on_setvalue >
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


