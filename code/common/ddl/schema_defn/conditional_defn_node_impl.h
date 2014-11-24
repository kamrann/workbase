// conditional_defn_node_impl.h

#ifndef __WB_DDL_CONDITIONAL_DEFN_NODE_IMPL_H
#define __WB_DDL_CONDITIONAL_DEFN_NODE_IMPL_H

#include "../ddl_types.h"
#include "defn_node.h"
#include "condition.h"

#include <vector>


namespace ddl {
	namespace detail {

		class conditional_defn_node_t
		{
		public:
			conditional_defn_node_t();

		private:
			struct component
			{
				cnd::condition cond;
				defn_node nd;
			};

		public:
			std::vector< component > components_;
			defn_node default_;
		};

	}
}


#endif


