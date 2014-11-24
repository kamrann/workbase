// instantiate.h
/*
Instantiation of schema.
All conditional nodes are evaluated, along with any other value-dependent components.
*/

#ifndef __WB_DDL_INSTANTIATE_H
#define __WB_DDL_INSTANTIATE_H

#include "../schema_defn/defn_node.h"
#include "sch_node.h"
#include "../values/value_node.h"
#include "../node_ref.h"


namespace ddl {

	class navigator;

	sch_node instantiate(defn_node const& defn, navigator nav, ref_resolver const& rr);

}


#endif



