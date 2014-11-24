// phys2d_domain_ddl.h

#ifndef __PHYS2D_DOMAIN_DDL_H
#define __PHYS2D_DOMAIN_DDL_H

#include "ddl/ddl.h"

#include "agent_spec_ddl.h"
#include "objective_ddl.h"


using namespace ddl;


struct phys2d_domain_schema
{
	defn_node get_defn(specifier& spc)
	{
		defn_node scenario = spc.enumeration()
			(define_enum_fixed{}("ground_based")/*TODO:*/)
			;

		defn_node agent_spec = agent_spec_schema().get_defn(spc);
		defn_node agent_spec_list = spc.list()
			(spc_min < size_t > { 1u })
			(spc_item{ agent_spec })
			;

		defn_node objective = objective_schema(node_ref{ agent_spec_list }).get_defn(spc);

		return spc.composite()(define_children{}
			("updates_per_sec", spc.integer()(spc_default < integer_defn_node::value_t > { 60 })(spc_min < integer_defn_node::value_t > { 1 }))
			("scenario", scenario)
			("agent_specs", agent_spec_list)
			("objective", objective)
			);
	}
};


#endif


