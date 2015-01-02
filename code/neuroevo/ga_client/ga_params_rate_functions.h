// ga_params_rate_functions.h

#ifndef __WB_GA_PARAMS_RATE_FNS_H
#define __WB_GA_PARAMS_RATE_FNS_H

#include "genetic_algorithm/rate_function.h"
#include "genetic_algorithm/fixed_rate_function.h"
#include "genetic_algorithm/generation_variable_rate_function.h"

//#include "params/param_accessor.h"
//#include "params/schema_builder.h"
#include "ddl/ddl.h"


namespace ga {

	struct fixed_rate_fn_defn
	{
/*		TODO: for this will need to make members non-static
		fixed_rate_fn_defn(double default_rate):
			_default(default_rate)
		{}
*/
		static ddl::defn_node get_defn(ddl::specifier& spc)
		{
			ddl::defn_node rate = spc.realnum("rate")
				(ddl::spc_range < ddl::realnum_defn_node::value_t > { 0.0, 1.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })	// todo:
				;

			return spc.composite("fixed_rate")(ddl::define_children{}
				("rate", rate)
				);
		}

		typedef ga::rate_fn_t result_t;

		static result_t generate(ddl::navigator nav)
		{
			return ga::fixed_rate_fn{ nav["rate"].get().as_real() };
		}

		double _default;
	};

	struct generation_variable_rate_fn_defn
	{
		/*		TODO: for this will need to make members non-static
		fixed_rate_fn_defn(double default_rate):
		_default(default_rate)
		{}
		*/
		static ddl::defn_node get_defn(ddl::specifier& spc)
		{
			ddl::defn_node init_rate = spc.realnum("initial_rate")
				(ddl::spc_range < ddl::realnum_defn_node::value_t > { 0.0, 1.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			ddl::defn_node cutoff_gens = spc.integer("cutoff_generations")
				(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 })
				(ddl::spc_default < ddl::integer_defn_node::value_t > { 100 })
				;

			ddl::defn_node cutoff_rate = spc.realnum("cutoff_rate")
				(ddl::spc_range < ddl::realnum_defn_node::value_t > { 0.0, 1.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			return spc.composite("var_gaussian_real_mut")(ddl::define_children{}
				("initial_rate", init_rate)
				("cutoff_generations", cutoff_gens)
				("cutoff_rate", cutoff_rate)
				);
		}

		typedef ga::rate_fn_t result_t;

		static result_t generate(ddl::navigator nav)
		{
			auto initial = nav["initial_rate"].get().as_real();
			size_t cutoff_generations = nav["cutoff_generations"].get().as_int();
			auto cutoff_rate = nav["cutoff_rate"].get().as_real();
			return ga::generation_variable_rate_fn{
				initial,
				ga::generation_variable_rate_fn::control_point{ cutoff_generations, cutoff_rate }
			};
		}

		double _default;
	};	

}


#endif


