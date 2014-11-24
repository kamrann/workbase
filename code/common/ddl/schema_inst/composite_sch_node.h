// composite_sch_node.h
/*
Options:
min/max entries
entry schema
*/

#ifndef __WB_DDL_COMPOSITE_SCHEMA_NODE_H
#define __WB_DDL_COMPOSITE_SCHEMA_NODE_H

#include "composite_sch_node_impl.h"
#include "sch_node_ptr.h"

#include <map>
#include <memory>


namespace ddl {

	class composite_sch_node
	{
	public:
		typedef detail::composite_sch_node_t impl_t;

	public:
		composite_sch_node();
		composite_sch_node(detail::composite_sch_node_ptr ptr);

	public:
		void add(node_name id, sch_node child);
		std::map< node_name, sch_node > children() const;
		bool has_child(node_name id) const;

	private:
		detail::composite_sch_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class sch_node;
	};

}


#endif


