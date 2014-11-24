// string_defn_node_impl.h

#ifndef __WB_DDL_STRING_DEFN_NODE_IMPL_H
#define __WB_DDL_STRING_DEFN_NODE_IMPL_H

#include "defn_node_base_impl.h"

#include <boost/optional.hpp>

#include <string>


namespace ddl {
	namespace detail {

		class string_defn_node_t:
			public defn_node_base_impl
		{
		public:
			typedef std::string value_t;
			typedef std::string charset_t;

		public:
			string_defn_node_t();

		public:
			size_t min_len_;
			boost::optional< size_t > max_len_;
			charset_t charset_;
			value_t default_;
		};

	}
}


#endif


