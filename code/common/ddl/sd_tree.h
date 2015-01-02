// sd_tree.h

#ifndef __WB_DDL_SCHEMA_DATA_TREE_H
#define __WB_DDL_SCHEMA_DATA_TREE_H

#include "schema_defn/defn_node.h"
#include "schema_inst/sch_node.h"
#include "values/value_node.h"
//#include "node_ref.h"

#include "util/gtree/generic_tree.hpp"


namespace ddl {

	struct sd_node_attribs
	{
		//node_ref defn;
		defn_node defn;
		sch_node schema;
		value_node data;
	};

	struct sd_edge_attribs
	{
		enum class ChildType {
			Composite,
			ListItem,
			Conditional,
		};

		ChildType type;
		// TODO: variant?
		node_name child_name;
		unsigned int item_idx;
	};

	typedef wb::gtree::generic_tree< sd_node_attribs, sd_edge_attribs > sd_tree;

	struct sd_node_ref;

	void update_sd_tree(sd_node_ref node, sd_tree& tree, bool include_root);
	void reset_node(sd_node_ref nref, sd_tree& tree);

}


#endif


