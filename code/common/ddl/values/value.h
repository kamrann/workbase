// value_node.h

#ifndef __WB_DDL_VALUE_H
#define __WB_DDL_VALUE_H

#include "../schema_inst/boolean_sch_node.h"
#include "../schema_inst/realnum_sch_node.h"
#include "../schema_inst/integer_sch_node.h"
#include "../schema_inst/enum_sch_node.h"
#include "../schema_inst/string_sch_node.h"

#include <boost/variant.hpp>


namespace ddl {

	typedef boost::variant<
		boolean_sch_node::value_t,
		realnum_sch_node::value_t,
		integer_sch_node::value_t,
		// vec2
		// random
		//enum_sch_node::value_t,
		std::vector< std::string >,	// enum strings
		string_sch_node::value_t
	>
	value;

	// TODO: temp legacy compatibility
	typedef std::vector< std::string > enum_param_val;

}


#endif


