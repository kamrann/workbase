// integer_defn_node_impl.h

#ifndef __WB_DDL_INTEGER_DEFN_NODE_IMPL_H
#define __WB_DDL_INTEGER_DEFN_NODE_IMPL_H

#include "defn_node_base_impl.h"

#include <boost/optional.hpp>

#include <cstdint>


namespace ddl {
	namespace detail {

		class integer_defn_node_t:
			public defn_node_base_impl
		{
		public:
			typedef intmax_t value_t;

		public:
			integer_defn_node_t();

		public:
			boost::optional< value_t > min_, max_;
			value_t default_;
		};
	
	}
}


#endif


