// list_sch_node_impl.h

#ifndef __WB_DDL_LIST_SCHEMA_NODE_IMPL_H
#define __WB_DDL_LIST_SCHEMA_NODE_IMPL_H

#include "sch_node.h"

#include <boost/optional.hpp>

#include <vector>


namespace ddl {
	namespace detail {

		class list_sch_node_t
		{
		public:
			list_sch_node_t();

		public:
			std::vector< sch_node > entry_schemas_;
			size_t min_entries_;
			boost::optional< size_t > max_entries_;
		};

	}
}


#endif


