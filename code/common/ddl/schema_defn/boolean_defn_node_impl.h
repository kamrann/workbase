// boolean_defn_node_impl.h

#ifndef __WB_DDL_BOOLEAN_DEFN_NODE_IMPL_H
#define __WB_DDL_BOOLEAN_DEFN_NODE_IMPL_H

#include "defn_node_base_impl.h"


namespace ddl {

	namespace detail {

		class boolean_defn_node_t:
			public defn_node_base_impl
		{
		public:
			typedef bool value_t;

		public:
			boolean_defn_node_t();

		public:
			// TODO: Ideally everything, even a default, could be a function of some value elsewhere in the
			// hierarchy, and so should be able to be specified by a named function.
			value_t default_;
		};

	}

}


#endif

