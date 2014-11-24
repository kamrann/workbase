// composite_sch_node_impl.h

#ifndef __WB_DDL_COMPOSITE_SCHEMA_NODE_IMPL_H
#define __WB_DDL_COMPOSITE_SCHEMA_NODE_IMPL_H

#include "../ddl_types.h"
#include "sch_node.h"

#include <map>


namespace ddl {
	namespace detail {

		class composite_sch_node_t
		{
		public:
			composite_sch_node_t();

//			void add(node_name id, node child);

		public:
			std::map< node_name, sch_node > children_;
		};

	}
}


#endif


