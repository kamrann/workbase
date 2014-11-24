// func_opt_domain_ddl.h

#ifndef __FUNC_OPT_DOMAIN_DDL_H
#define __FUNC_OPT_DOMAIN_DDL_H

#include "ddl/ddl.h"

#include "funcopt_function_ddl.h"
//#include "funcopt_mutation_ddl.h"


using namespace ddl;


struct func_opt_domain_schema
{
	defn_node get_defn(specifier& spc)
	{
		defn_node function = funcopt_function_schema().get_defn(spc);

		// TODO: Crossover

//		defn_node mutation = funcopt_mutation_schema().get_defn(spc);

		return spc.composite()(define_children{}
			("function", function)
			// TODO: crossover
//			("mutation", mutation)
			);
	}
};


#endif


