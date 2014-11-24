// funcopt_function_ddl.h

#ifndef __FUNCOPT_FUNCTION_DDL_H
#define __FUNCOPT_FUNCTION_DDL_H

#include "ddl/ddl.h"

#include "funcopt_ackley_ddl.h"
#include "funcopt_rosenbrock_ddl.h"


using namespace ddl;


struct funcopt_function_schema
{
	defn_node get_defn(specifier& spc)
	{
		defn_node type = spc.enumeration()
			(define_enum_fixed{}("ackley")("rosenbrock"))
			;

		defn_node cond_nd = spc.conditional()
			(spc_condition{ cnd::equal{ node_ref{ type }, "ackley" }, funcopt_ackley_schema().get_defn(spc) })
			(spc_condition{ cnd::equal{ node_ref{ type }, "rosenbrock" }, funcopt_rosenbrock_schema().get_defn(spc) })
			;

		return spc.composite()(define_children{}
			("type", type)
			("contents", cond_nd)
			);
	}
};


#endif


