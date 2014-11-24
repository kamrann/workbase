// objective_ddl.h

#ifndef __OBJECTIVE_DDL_H
#define __OBJECTIVE_DDL_H

#include "ddl/ddl.h"

#include "value_objective_ddl.h"


using namespace ddl;


struct objective_schema
{
	objective_schema(node_ref spec_list_ref):
		spec_list_ref_(spec_list_ref)
	{}

	defn_node get_defn(specifier& spc)
	{
		defn_node obj_type = spc.enumeration()(define_enum_fixed{}("single")("pareto"));

		defn_node valobj = value_objective_schema(spec_list_ref_).get_defn(spc);
		defn_node valobj_list = spc.list()
			(spc_item{ valobj })
			(spc_min< size_t >{ 1u })
			;
		defn_node cond_nd = spc.conditional()
			(spc_condition{ cnd::equal{ node_ref{ obj_type }, "single" }, valobj })
			(spc_condition{ cnd::equal{ node_ref{ obj_type }, "pareto" }, valobj_list })
			;

		defn_node objective = spc.composite()(define_children{}
			("type", obj_type)
			("unnamed", cond_nd)
			);
		return objective;
	}

	node_ref spec_list_ref_;
};


#endif


