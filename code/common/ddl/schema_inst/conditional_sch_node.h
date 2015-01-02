// conditional_sch_node.h

#ifndef __WB_DDL_CONDITIONAL_SCHEMA_NODE_H
#define __WB_DDL_CONDITIONAL_SCHEMA_NODE_H

#include "conditional_sch_node_impl.h"
#include "sch_node_ptr.h"

#include <map>
#include <memory>


namespace ddl {

	class conditional_sch_node
	{
	public:
		typedef detail::conditional_sch_node_t impl_t;

	public:
		conditional_sch_node();
		conditional_sch_node(detail::conditional_sch_node_ptr ptr);

	public:
		bool is_active() const;

	private:
		detail::conditional_sch_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class sch_node;
	};

}


#endif


