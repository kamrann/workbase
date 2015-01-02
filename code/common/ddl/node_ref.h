// node_ref.h

#ifndef __WB_DDL_NODE_REF_H
#define __WB_DDL_NODE_REF_H

#include "schema_defn/defn_node.h"


namespace ddl {

	struct node_ref
	{
		defn_node nd;

		enum class Resolution {
			// TODO: Will probably need to provide more control of the resolving context, (for example,
			// "sibling level", or "descendent of parent"), but for now, just separate into two types:
			// 1- assumed unique reference
			// 2- maching node anywhere in the hierarchy
			Unique,
			Any,
		};

		Resolution res;

		node_ref():
			nd{},
			res{ Resolution::Unique }
		{}

		node_ref(defn_node n, Resolution r = Resolution::Unique):
			nd{ n },
			res{ r }
		{}
	};


	class navigator;

	// This returns null navigator if the resolution did not yield a single unique node
	navigator nav_to_ref(node_ref const& ref, navigator nav);

}


#endif


