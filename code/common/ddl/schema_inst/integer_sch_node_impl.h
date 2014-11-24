// integer_sch_node_impl.h

#ifndef __WB_DDL_INTEGER_SCHEMA_NODE_IMPL_H
#define __WB_DDL_INTEGER_SCHEMA_NODE_IMPL_H

#include <boost/optional.hpp>

#include <cstdint>


namespace ddl {
	namespace detail {

		class integer_sch_node_t
		{
		public:
			typedef intmax_t value_t;

		public:
			integer_sch_node_t();

		public:
			boost::optional< value_t > min_, max_;
			value_t default_;
		};
	
	}
}


#endif


