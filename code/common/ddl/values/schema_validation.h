// schema_validation.h

#ifndef __WB_DDL_SCHEMA_VALIDATION_H
#define __WB_DDL_SCHEMA_VALIDATION_H

#include "../sd_node_ref.h"


namespace ddl {

	void adjust_to_schema(sd_node_ref nref, sd_tree& tree);
	bool fits_schema(value_node const& vals, sch_node const& sch);

}


#endif


