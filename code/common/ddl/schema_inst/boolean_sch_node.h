// boolean_sch_node.h
/*
Options:
default
*/

#ifndef __WB_DDL_BOOLEAN_SCHEMA_NODE_H
#define __WB_DDL_BOOLEAN_SCHEMA_NODE_H

#include "boolean_sch_node_impl.h"
#include "sch_node_ptr.h"

#include <memory>


namespace ddl {

	class boolean_sch_node
	{
	public:
		typedef detail::boolean_sch_node_t impl_t;
		typedef impl_t::value_t value_t;

	public:
		boolean_sch_node();
		boolean_sch_node(detail::boolean_sch_node_ptr ptr);

	public:
		void default(value_t val);
		value_t default() const;

	public:
		detail::boolean_sch_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class sch_node;
	};

}


#endif

