// list_defn_node_impl.h

#ifndef __WB_DDL_LIST_DEFN_NODE_IMPL_H
#define __WB_DDL_LIST_DEFN_NODE_IMPL_H

#include "defn_node.h"
#include "defn_node_base_impl.h"

#include <boost/optional.hpp>


namespace ddl {
	namespace detail {

		class list_defn_node_t:
			public defn_node_base_impl
		{
		public:
			list_defn_node_t();

		public:
			defn_node entry_schema_;
			size_t min_entries_;
			boost::optional< size_t > max_entries_;
		};

	}
}


#endif


