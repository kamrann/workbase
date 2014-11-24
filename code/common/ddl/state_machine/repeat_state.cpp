// repeat_state.cpp

#include "repeat_state.h"


namespace ddl {
	namespace fsm {

		void repeat::on_add(clsm::ev_cmd< repeat_add_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto nav = editor.nav;

			auto vnode = nav.get();
			auto list = vnode.as_list();
			list.push_back(value_node{});
			vnode = list;

			// TODO: Overkill
			editor.reload_pt_schema();

			editor.output("instance added");
		}

		void repeat::on_remove(clsm::ev_cmd< repeat_remove_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto nav = editor.nav;

			auto vnode = nav.get();
			auto list = vnode.as_list();

			if(cmd.index < list.size())
			{
				list.erase(std::begin(list) + cmd.index);
				vnode = list;

				// TODO: Overkill
				editor.reload_pt_schema();

				editor.output("instance removed");
			}
			else
			{
				editor.output("instance does not exist");
			}
		}

		std::string repeat::get_prompt() const
		{
			auto location = context< paramtree_editor >().nav.where();
			return location.to_string() + " [list]: ";
		}

	}
}




