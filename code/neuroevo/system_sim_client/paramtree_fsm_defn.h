// paramtree_fsm_defn.h

#ifndef __WB_SYSSIM_CLIENT_PARAMTREE_FSM_DEFN_H
#define __WB_SYSSIM_CLIENT_PARAMTREE_FSM_DEFN_H

#include "params/param_tree.h"
#include "params/param_accessor.h"

#include <boost/statechart/state_machine.hpp>
#include <boost/mpl/list.hpp>

#include <string>
#include <functional>


namespace prm {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct base_node_state;

		struct paramtree_editor: sc::state_machine < paramtree_editor, base_node_state >
		{
			typedef mpl::list<> commands;

			paramtree_editor(
				prm::param_tree _pt,
				prm::schema::schema_provider_map_handle _provider,
				std::function< void(std::string) > _output_sink
				);

			// External querying
			std::string get_prompt() const;

			// For state machine internal use
			// Querying
			ParamType current_node_type() const;

			// Actions
			void output(std::string txt) const;
			void reload_pt_schema();

			prm::param_tree ptree;
			prm::param_accessor acc;
			prm::schema::schema_provider_map_handle provider;

			std::function< void(std::string) > output_sink;
		};

	}
}


#endif


