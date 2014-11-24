// realnum_sch_node_impl.h

#ifndef __WB_DDL_REALNUM_SCHEMA_NODE_IMPL_H
#define __WB_DDL_REALNUM_SCHEMA_NODE_IMPL_H

#include <boost/optional.hpp>


namespace ddl {
	namespace detail {

		class realnum_sch_node_t
		{
		public:
			typedef double value_t;

		public:
			realnum_sch_node_t();

		public:
			boost::optional< value_t > min_, max_;
			value_t default_;
		};

	}
}


#endif


