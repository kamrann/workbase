// funcopt_ackley_ddl.h

#ifndef __FUNCOPT_ACKLEY_DDL_H
#define __FUNCOPT_ACKLEY_DDL_H

#include "ddl/ddl.h"


using namespace ddl;


struct funcopt_ackley_schema
{
	defn_node get_defn(specifier& spc)
	{
		defn_node dims = spc.integer()
			(spc_min< integer_defn_node::value_t >{ 1 })
			(spc_default< integer_defn_node::value_t >{ 2 })
			;
		defn_node lower = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ -40.0 })
			;
		defn_node upper = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ 40.0 })
			;
		// TODO: Should these constants be bounded?
		defn_node a = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ 20.0 })
			;
		defn_node b = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ 0.2 })
			;
		defn_node c_pi = spc.realnum()
			(spc_default< realnum_defn_node::value_t >{ 2.0 })
			;

		return spc.composite()(define_children{}
			("dimensions", dims)
			("lower_bound", lower)
			("upper_bound", upper)
			("a", a)
			("b", b)
			("c(pi)", c_pi)
			);
	}

	function_opt_domain::function_defn generate(navigator nav) const
	{
		auto dim = nav["dimensions"].get().as_int();
		auto lower = nav["lower_bound"].as_real();
		auto upper = nav["upper_bound"].as_real();
		auto a = nav["a"].as_real();
		auto b = nav["b"].as_real();
		auto c = nav["c(pi)"].as_real() * M_PI;

		return function_opt_domain::function_defn(ackley(
			dim,
			lower,
			upper,
			a,
			b,
			c
			));
	}
};


#endif


