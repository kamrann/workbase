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
#include "../sd_node_ref.h"


namespace ddl {

	class navigator;

	sch_node instantiate(defn_node const& defn, navigator nav, ref_resolver const& rr);

	boolean_sch_node instantiate(boolean_defn_node const& nd, navigator nav, ref_resolver const& rr);
	integer_sch_node instantiate(integer_defn_node const& nd, navigator nav, ref_resolver const& rr);
	realnum_sch_node instantiate(realnum_defn_node const& nd, navigator nav, ref_resolver const& rr);
	string_sch_node instantiate(string_defn_node const& nd, navigator nav, ref_resolver const& rr);
	enum_sch_node instantiate(enum_defn_node const& nd, navigator nav, ref_resolver const& rr);

}


#endif



