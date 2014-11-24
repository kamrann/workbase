// defn_node_holder.h

#ifndef __WB_DDL_DEFN_NODE_HOLDER_H
#define __WB_DDL_DEFN_NODE_HOLDER_H

#include "../ddl_types.h"
#include "boolean_defn_node.h"
#include "integer_defn_node.h"
#include "realnum_defn_node.h"
#include "string_defn_node.h"
#include "enum_defn_node.h"
#include "list_defn_node.h"
#include "composite_defn_node.h"
#include "conditional_defn_node.h"

#include <boost/variant.hpp>


namespace ddl {
	namespace detail {

		typedef boost::variant <
			boolean_defn_node,
			integer_defn_node,
			realnum_defn_node,
			string_defn_node,
			enum_defn_node,
			list_defn_node,
			composite_defn_node,
			conditional_defn_node
		>
		defn_node_var_t;

		struct defn_holder
		{
			template < typename T >
			defn_holder(T const& v): var_{ v }
			{}

			defn_node_var_t var_;
		};

	}
}


#endif


