// realnum_defn_node_impl.h

#ifndef __WB_DDL_REALNUM_DEFN_NODE_IMPL_H
#define __WB_DDL_REALNUM_DEFN_NODE_IMPL_H

#include "defn_node_base_impl.h"

#include <boost/optional.hpp>


namespace ddl {
	namespace detail {

		class realnum_defn_node_t:
			public defn_node_base_impl
		{
		public:
			typedef double value_t;

		public:
			realnum_defn_node_t();

		public:
			boost::optional< value_t > min_, max_;
			value_t default_;
		};

	}
}


#endif


