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
			//value_node _data,
			sd_tree _data,
			std::function< void(std::string) > _output_sink,
			std::string _quit_str
			):
			defn{ _defn },
//			schema{},
			data{ std::move(_data) },
			nav{ &data, sd_node_ref{ data.get_root() } },
			output_sink{ _output_sink },
			quit_str{ _quit_str }
		{
//			reset_node(nav.get_ref(), data);
			update_sd_tree(nav.get_ref(), data, true);
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
			ref_resolver rr = &resolve_reference;

			auto& editor = context< paramtree_editor >();
/*			auto pos = editor.nav.where();
			schema = instantiate(editor.defn, navigator{ editor.data }, rr);
			adjust_to_schema(editor.data, schema);
			editor.nav = navigator{ editor.data };
			editor.nav = editor.nav[pos];	// TODO: may no longer exist, should move to deepest valid subpath
*/

			auto pos = editor.nav.where();
			update_sd_tree(sd_node_ref{ data.get_root() }, data, false);
			editor.nav = editor.nav[pos];
		}

	}
}




