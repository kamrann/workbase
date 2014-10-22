// paramtree_fsm_defn.cpp

#include "paramtree_fsm_defn.h"
#include "clsm.h"

#include "params/value_emitter.h"
#include "params/param_yaml.h"

#include "util/gtree/generic_tree_depthfirst.hpp"

#include <fstream>


namespace prm {
	namespace fsm {

		paramtree_editor::paramtree_editor(
			prm::param_tree _pt,
			prm::schema::schema_provider_map_handle _provider,
			std::function< void(std::string) > _output_sink
			):
			ptree{ std::move(_pt) },
			acc{ &ptree },
			provider{ _provider },
			output_sink{ _output_sink }
		{

		}

		std::string paramtree_editor::get_prompt() const
		{
			return state_cast<clsm::i_cmd_state const&>().get_prompt();
		}

		ParamType paramtree_editor::current_node_type() const
		{
			auto pr = acc.param_here();
			return pr.type;
		}

		void paramtree_editor::output(std::string txt) const
		{
			output_sink(std::move(txt));
		}

		void paramtree_editor::reload_pt_schema()
		{
			auto pos = acc.where();
			auto rootname = ptree.rootname();
			auto sch = (*provider)[rootname](acc);

			// Make a deep copy of the accessor/param tree
			param_tree pt_copy{ ptree };
			param_accessor acc_copy{ &pt_copy };
			acc_copy.move_to(pos);

			ptree = param_tree::generate_from_schema(sch, acc_copy, provider);

			acc = param_accessor{ &ptree };
			acc.move_to(pos);
		}

	}
}




