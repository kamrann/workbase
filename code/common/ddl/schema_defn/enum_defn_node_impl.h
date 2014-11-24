// enum_defn_node_impl.h

#ifndef __WB_DDL_ENUM_DEFN_NODE_IMPL_H
#define __WB_DDL_ENUM_DEFN_NODE_IMPL_H

#include "defn_node_base_impl.h"

#include <boost/optional.hpp>

#include <string>
#include <vector>
#include <set>


namespace ddl {

	class navigator;

	namespace detail {

		class enum_defn_node_t:
			public defn_node_base_impl
		{
		public:
			typedef std::string enum_value_t;
			// TODO: typedef std::set< enum_value_t > value_t;
			typedef std::vector< enum_value_t > value_t;
			typedef std::set< enum_value_t > enum_set_t;

			// TODO: Eventually may want to have named functions, with generic return type
			typedef std::function< enum_set_t(navigator) > enum_values_fn_t;

		public:
			enum_defn_node_t();

//			void add_option(enum_value_t val);

		public:
			enum_values_fn_t enum_values_fn_;
			size_t min_sel_;
			boost::optional< size_t > max_sel_;
		};

	}
}


#endif


