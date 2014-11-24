// composite_defn_node.h
/*
Options:
min/max entries
entry schema
*/

#ifndef __WB_DDL_COMPOSITE_DEFN_NODE_H
#define __WB_DDL_COMPOSITE_DEFN_NODE_H

#include "composite_defn_node_impl.h"
#include "defn_node_ptr.h"

#include <map>
#include <memory>


namespace ddl {

	class composite_defn_node
	{
	public:
		typedef detail::composite_defn_node_t impl_t;

	public:
		composite_defn_node();
		composite_defn_node(detail::composite_defn_node_ptr ptr);

	public:
		void add(node_name id, defn_node child);

	public:
		std::map< node_name, defn_node > children() const;

	private:
		detail::composite_defn_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class defn_node;
	};

}


#endif


