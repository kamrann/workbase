// composite_defn_node_impl.h

#ifndef __WB_DDL_COMPOSITE_DEFN_NODE_IMPL_H
#define __WB_DDL_COMPOSITE_DEFN_NODE_IMPL_H

#include "../ddl_types.h"
#include "defn_node.h"
#include "defn_node_base_impl.h"

#include <map>


namespace ddl {
	namespace detail {

		class composite_defn_node_t:
			public defn_node_base_impl
		{
		public:
			composite_defn_node_t();

//			void add(defn_node_id id, defn_node child);

		public:
			std::map< node_name, defn_node > children_;
		};

	}
}


#endif


