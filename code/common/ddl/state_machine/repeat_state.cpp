// repeat_state.cpp

#include "repeat_state.h"


namespace ddl {
	namespace fsm {

		void repeat::on_add(clsm::ev_cmd< repeat_add_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto nav = editor.nav;

			sd_edge_attribs edge;
			edge.type = sd_edge_attribs::ChildType::ListItem;
			edge.item_idx = nav.tree_->child_count(nav.pos_.nd);
			sd_node_attribs node;
			//node.defn = ? ;
			// - this is dealt with within sd_tree update below...

			// TODO: Temp. Maybe just change nav tree pointer to non const, but really need to decide exactly what
			// nav should be used for, and when sd_tree should be used directly.
			auto tree = const_cast<sd_tree*>(nav.tree_);
			tree->add_node(nav.pos_.nd, edge, node);

/*			auto vnode = nav.get();
			auto list = vnode.as_list();
			list.push_back(value_node{});
			vnode = list;
*/
			// TODO: Overkill
			editor.reload_pt_schema();

			editor.output("instance added");
		}

		void repeat::on_remove(clsm::ev_cmd< repeat_remove_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto nav = editor.nav;

//			auto vnode = nav.get();
//			auto list = vnode.as_list();

			if(cmd.index < nav.list_num_items())//list.size())
			{
//				list.erase(std::begin(list) + cmd.index);
//				vnode = list;

				// TODO:
				auto tree = const_cast<sd_tree*>(nav.tree_);
				auto c = tree->get_nth_child(nav.pos_.nd, cmd.index).first;
				tree->remove_branch(c);

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




