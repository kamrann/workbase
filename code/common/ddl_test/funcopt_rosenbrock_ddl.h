// funcopt_rosenbrock_ddl.h

#ifndef __FUNCOPT_ROSENBROCK_DDL_H
#define __FUNCOPT_ROSENBROCK_DDL_H

#include "ddl/ddl.h"


using namespace ddl;


struct funcopt_rosenbrock_schema
{
	defn_node get_defn(specifier& spc)
	{
		defn_node dims = spc.integer()
			(spc_min< integer_defn_node::value_t >{ 1 })
			(spc_default< integer_defn_node::value_t >{ 2 })
			;
		defn_node lower = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ -50.0 })
			;
		defn_node upper = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ 50.0 })
			;
		// TODO: Should these constants be bounded?
		defn_node a = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ 1.0 })
			;
		defn_node b = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ 100.0 })
			;

		return spc.composite()(define_children{}
			("dimensions", dims)
			("lower_bound", lower)
			("upper_bound", upper)
			("a", a)
			("b", b)
			);
	}
};


#endif


