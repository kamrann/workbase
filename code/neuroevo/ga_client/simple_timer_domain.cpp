// simple_timer_domain.cpp

#include "simple_timer_domain.h"

#include "genetic_algorithm/param_spec/real_valued_mutation_ps.h"


namespace ga {
	namespace domain {
		namespace timer {

			simple_timer_domain_defn::simple_timer_domain_defn()
			{
				gene_mutation_defn_.register_option(
					"linear",
					linear_real_gene_mutation_defn< double >::get_defn,
					linear_real_gene_mutation_defn< double >::generate
					);
				gene_mutation_defn_.register_option(
					"gaussian",
					gaussian_real_gene_mutation_defn< double >::get_defn,
					gaussian_real_gene_mutation_defn< double >::generate
					);
				gene_mutation_defn_.register_option(
					"variable_gaussian",
					variable_gaussian_real_gene_mutation_defn< double >::get_defn,
					variable_gaussian_real_gene_mutation_defn< double >::generate
					);
			}

			ddl::defn_node simple_timer_domain_defn::get_defn(ddl::specifier& spc)
			{
				ddl::defn_node temp_mutation = gene_mutation_defn_.get_defn(spc);

				return spc.composite("simple_timer")(ddl::define_children{}
					("temp_mutation", temp_mutation)
					);
			}
			
			std::shared_ptr< ga::i_problem_domain > simple_timer_domain_defn::generate(ddl::navigator nav) const
			{
				auto gene_mut_fn = gene_mutation_defn_.generate(nav["temp_mutation"]);

				return std::make_shared< simple_timer_domain >(gene_mut_fn);
			}

		}
	}
}

