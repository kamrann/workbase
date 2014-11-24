// problem_domain_ddl.h

#ifndef __PROBLEM_DOMAIN_DDL_H
#define __PROBLEM_DOMAIN_DDL_H

#include "ddl/ddl.h"

#include "func_opt_domain_ddl.h"
//#include "phys2d_domain_ddl.h"


using namespace ddl;


struct problem_domain_schema
{
	defn_node get_defn(specifier& spc)
	{
		defn_node domain_type = spc.enumeration()
			(define_enum_fixed{}("func_opt")/*TODO:*/)
			;

		defn_node func_opt = func_opt_domain_schema().get_defn(spc);

		defn_node cond_nd = spc.conditional()
			(spc_condition{ cnd::equal{ node_ref{ domain_type }, "func_opt" }, func_opt })
			;

		return spc.composite()(define_children{}
			("type", domain_type)
			("domain", cond_nd)
			);
	}
};


#endif


