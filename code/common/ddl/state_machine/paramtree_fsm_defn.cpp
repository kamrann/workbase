// paramtree_fsm_defn.cpp

#include "paramtree_fsm_defn.h"
#include "clsm/clsm.h"

#include "../terminal_io/value_emitter.h"
#include "../node_ref.h"
#include "../schema_inst/instantiate.h"

#include <fstream>


namespace ddl {
	namespace fsm {

		paramtree_editor::paramtree_editor(
			defn_node _defn,
			std::function< void(std::string) > _output_sink,
			std::string _quit_str
			):
			defn{ _defn },
			schema{},
			data{},
			nav{},
			output_sink{ _output_sink },
			quit_str{ _quit_str }
		{
			reload_pt_schema();
		}

		std::string paramtree_editor::get_prompt() const
		{
			return state_cast<clsm::i_cmd_state const&>().get_prompt();
		}

/*		ParamType paramtree_editor::current_node_type() const
		{
			auto pr = acc.param_here();
			return pr.type;
		}
*/
		void paramtree_editor::output(std::string txt) const
		{
			output_sink(std::move(txt));
		}

		void paramtree_editor::reload_pt_schema()
		{
/*			auto pos = acc.where();
			auto rootname = ptree.rootname();
			auto sch = (*provider)[rootname](acc);

			// Make a deep copy of the accessor/param tree
			param_tree pt_copy{ ptree };
			param_accessor acc_copy{ &pt_copy };
			acc_copy.move_to(pos);

			ptree = param_tree::generate_from_schema(sch, acc_copy, provider);

			acc = param_accessor{ &ptree };
			acc.move_to(pos);
			*/

			ref_resolver rr = &resolve_reference;

			auto& editor = context< paramtree_editor >();
			auto pos = editor.nav.where();
			schema = instantiate(editor.defn, navigator{ editor.data }, rr);
			adjust_to_schema(editor.data, schema);
			editor.nav = navigator{ editor.data };
			editor.nav = editor.nav[pos];	// TODO: may no longer exist, should move to deepest valid subpath
		}

	}
}




