// repeat_state.cpp

#include "repeat_state.h"


namespace prm {
	namespace fsm {

		sc::result repeat::dispatch(repeat_add_cmd const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			// Make a copy of the accessor/param tree
			auto pt_copy = param_tree{ editor.ptree };
			auto acc_copy = param_accessor{ &pt_copy };
			acc_copy.move_to(acc.where());

			editor.ptree.add_repeat_instance(acc.node_here(), acc_copy, editor.provider);

			// TODO: Overkill
			editor.reload_pt_schema();

			editor.output("instance added");
			return discard_event();
		}

		sc::result repeat::dispatch(repeat_remove_cmd const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			auto success = editor.ptree.remove_repeat_instance(acc.node_here(), cmd.index);
			if(success)
			{
				editor.output("instance removed");
			}
			else
			{
				editor.output("instance does not exist");
			}
			// TODO: deal with case where current location no longer exists
			// (see dispatch for load command)
			return discard_event();
		}

		std::string repeat::get_prompt() const
		{
			auto location = context< paramtree_editor >().acc.where();
			return location.to_string() + " [rpt]: ";
		}

	}
}




