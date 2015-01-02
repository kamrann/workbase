// enum_sch_node_impl.h

#ifndef __WB_DDL_ENUM_SCHEMA_NODE_IMPL_H
#define __WB_DDL_ENUM_SCHEMA_NODE_IMPL_H

#include "../schema_defn/enum_defn_node.h"

#include <boost/optional.hpp>

#include <string>
#include <vector>
#include <set>


namespace ddl {
	namespace detail {

		class enum_sch_node_t
		{
		public:
			typedef ddl::enum_defn_node::enum_value_t enum_value_t;
			typedef ddl::enum_defn_node::value_t value_t;
			typedef ddl::enum_defn_node::str_value_t str_value_t;
			typedef ddl::enum_defn_node::enum_set_t enum_set_t;

		public:
			enum_sch_node_t();

//			void add_option(enum_value_t val);

		public:
			enum_set_t enum_values_;
			size_t min_sel_;
			boost::optional< size_t > max_sel_;
			value_t default_;
		};

	}
}


#endif


