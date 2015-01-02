// real_valued_mutation_ps.h

#ifndef __WB_GA_REAL_VALUED_MUTATION_PS_H
#define __WB_GA_REAL_VALUED_MUTATION_PS_H

#include "../real_valued_mutation.h"

//#include "params/param_accessor.h"
//#include "params/schema_builder.h"
#include "ddl/ddl.h"


namespace ga {

	template < typename Rep >
	struct linear_real_gene_mutation_defn
	{
		static ddl::defn_node get_defn(ddl::specifier& spc)
		{
			ddl::defn_node max_delta = spc.realnum("max_delta")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.1 })
				;

			return spc.composite("linear_real_mut")(ddl::define_children{}
				("max_delta", max_delta)
				);
		}

		typedef Rep gene_t;
		typedef ga::gene_mutation< gene_t > result_t;

		static result_t generate(ddl::navigator nav)
		{
			auto max_delta = nav["max_delta"].get().as_real();
			return ga::real_valued_gene< gene_t >::linear_mutation{ max_delta };
		}
	};

	template < typename Rep >
	struct gaussian_real_gene_mutation_defn
	{
		static ddl::defn_node get_defn(ddl::specifier& spc)
		{
			ddl::defn_node std_dev = spc.realnum("std_dev")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { std::numeric_limits< double >::min() })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			return spc.composite("gaussian_real_mut")(ddl::define_children{}
				("std_dev", std_dev)
				);
		}

		typedef Rep gene_t;
		typedef ga::gene_mutation< gene_t > result_t;

		static result_t generate(ddl::navigator nav)
		{
			auto std_dev = nav["std_dev"].get().as_real();
			return ga::real_valued_gene< gene_t >::gaussian_mutation{ std_dev };
		}
	};

	template < typename Rep >
	struct variable_gaussian_real_gene_mutation_defn
	{
		static ddl::defn_node get_defn(ddl::specifier& spc)
		{
			ddl::defn_node init_std_dev = spc.realnum("initial_std_dev")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { std::numeric_limits< double >::min() })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			ddl::defn_node cutoff_gens = spc.integer("cutoff_generations")
				(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 })
				(ddl::spc_default < ddl::integer_defn_node::value_t > { 100 })
				;

			ddl::defn_node cutoff_std_dev = spc.realnum("cutoff_std_dev")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { std::numeric_limits< double >::min() })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			return spc.composite("var_gaussian_real_mut")(ddl::define_children{}
				("initial_std_dev", init_std_dev)
				("cutoff_generations", cutoff_gens)
				("cutoff_std_dev", cutoff_std_dev)
				);
		}

		typedef Rep gene_t;
		typedef ga::gene_mutation< gene_t > result_t;

		static result_t generate(ddl::navigator nav)
		{
			auto initial = nav["initial_std_dev"].get().as_real();
			size_t cutoff_generations = nav["cutoff_generations"].get().as_int();
			auto cutoff = nav["cutoff_std_dev"].get().as_real();
			return ga::real_valued_gene< gene_t >::variable_gaussian_mutation{ initial, { cutoff_generations, cutoff } };
		}
	};

}


#endif






