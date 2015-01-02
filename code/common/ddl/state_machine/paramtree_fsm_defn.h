// paramtree_fsm_defn.h

#ifndef __WB_SYSSIM_CLIENT_PARAMTREE_FSM_DEFN_H
#define __WB_SYSSIM_CLIENT_PARAMTREE_FSM_DEFN_H

#include "../schema_defn/defn_node.h"
#include "../schema_inst/sch_node.h"
#include "../values/value_node.h"
#include "../values/navigator.h"

#include "clsm/clsm.h"
//#include "params/param_tree.h"
//#include "params/param_accessor.h"

#include <boost/statechart/state_machine.hpp>
#include <boost/mpl/list.hpp>

#include <string>
#include <functional>


namespace ddl {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct base_node_state;

		struct paramtree_editor: clsm::state_machine < paramtree_editor, base_node_state >
		{
			paramtree_editor(
				defn_node _defn,
//				value_node _data,
				sd_tree _data,
				std::function< void(std::string) > _output_sink,
				std::string _quit_str = ":q"
				);

			// External querying
			std::string get_prompt() const;

			// For state machine internal use
			// Querying
			//ParamType current_node_type() const;

			// Actions
			void output(std::string txt) const;
			void reload_pt_schema();

			defn_node defn;
//			sch_node schema;
//			value_node data;
			sd_tree data;
			navigator nav;

			std::function< void(std::string) > output_sink;
			std::string quit_str;
		};

	}
}


#endif


