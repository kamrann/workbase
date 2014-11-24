// node_ref.h

#ifndef __WB_DDL_NODE_REF_H
#define __WB_DDL_NODE_REF_H

#include "schema_defn/defn_node.h"

#include <functional>


namespace ddl {

	struct node_ref
	{
		defn_node nd;
	};

	class value_node;
	class navigator;

	typedef std::function< value_node(node_ref const&, navigator) > ref_resolver;

	navigator nav_to_ref(node_ref const& ref, navigator nav);
	value_node resolve_reference(node_ref const& ref, navigator nav);

}


#endif


