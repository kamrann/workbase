// output.h

#ifndef __WB_DDL_OUTPUT_H
#define __WB_DDL_OUTPUT_H

#include "schema_inst/sch_node.h"
#include "values/value_node.h"

#include <ostream>


namespace ddl {

	std::ostream& operator<< (std::ostream& os, sch_node const& schema);
	std::ostream& operator<< (std::ostream& os, value_node const& val);

}


#endif



