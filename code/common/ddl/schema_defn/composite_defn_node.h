// composite_defn_node.h
/*
Options:
min/max entries
entry schema
*/

#ifndef __WB_DDL_COMPOSITE_DEFN_NODE_H
#define __WB_DDL_COMPOSITE_DEFN_NODE_H

#include "defn_node_base_impl.h"
#include "defn_node.h"

#include <map>


namespace ddl {

	class composite_defn_node:
		public detail::defn_node_base_impl
	{
	public:
		composite_defn_node();

	public:
		void add(node_name id, defn_node child);

		std::map< node_name, defn_node > children() const;

	private:
		std::map< node_name, defn_node > children_;

		friend class defn_node;
	};

}


#endif


