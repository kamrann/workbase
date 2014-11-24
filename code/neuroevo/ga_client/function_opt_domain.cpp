// function_opt_domain.cpp

#include "function_opt_domain.h"
#include "ackley.h"
#include "rosenbrock.h"
#include "ga_params.h"	// TEMP for Components enum

#include "genetic_algorithm/param_spec/real_valued_mutation_ps.h"

#include "params/schema_builder.h"
#include "params/param_accessor.h"


namespace sb = prm::schema;

namespace ga {
	namespace domain {
		namespace fnopt {

			function_opt_domain_defn::function_opt_domain_defn()//:
				//	gene_mutation_defn("mutation")
			{
				gene_mutation_defn.register_option(
					"linear",
					linear_real_gene_mutation_defn< double >::update_schema_provider,
					linear_real_gene_mutation_defn< double >::generate
					);
				gene_mutation_defn.register_option(
					"gaussian",
					gaussian_real_gene_mutation_defn< double >::update_schema_provider,
					gaussian_real_gene_mutation_defn< double >::generate
					);
				gene_mutation_defn.register_option(
					"variable_gaussian",
					variable_gaussian_real_gene_mutation_defn< double >::update_schema_provider,
					variable_gaussian_real_gene_mutation_defn< double >::generate
					);

				func_defn.register_option(
					"ackley",
					std::make_shared< functions::ackley_defn >()
					);
				func_defn.register_option(
					"rosenbrock",
					std::make_shared< functions::rosenbrock_defn >()
					);
			}

			void function_opt_domain_defn::update_schema_provider_for_crossover_op(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) //const
			{
				auto path = prefix;

				(*provider)[path] = [=](prm::param_accessor acc)
				{
					auto s = sb::list(path.leaf().name());
					return s;
				};
			}

			void function_opt_domain_defn::update_schema_provider_for_mutation_op(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) //const
			{
				auto path = prefix;

				gene_mutation_defn.update_schema_provider(provider, path + std::string{ "gene_mutation" });

				(*provider)[path] = [=](prm::param_accessor acc)
				{
					auto s = sb::list(path.leaf().name());
					sb::append(s, provider->at(path + std::string{ "gene_mutation" })(acc));
					return s;
				};
			}

			void function_opt_domain_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) //const
			{
				func_defn.update_schema_provider(provider, prefix);
				
/*				auto path = prefix;

				(*provider)[path] = [=](prm::param_accessor acc)
				{
					auto s = sb::list(path.leaf().name());
					// TODO: stuff
					return s;
				};
*/			}

			void function_opt_domain_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, int component)// const
			{
				switch(component)
				{
					case Components::CrossoverOp:
					update_schema_provider_for_crossover_op(provider, prefix);
					break;
					case Components::MutationOp:
					update_schema_provider_for_mutation_op(provider, prefix);
					break;
				}
			}
			
			std::shared_ptr< ga::i_problem_domain > function_opt_domain_defn::generate(prm::param_accessor acc) const
			{
				function_opt_domain::function_defn fdef = func_defn.generate(acc);

				auto gene_mut_fn = gene_mutation_defn.generate(acc("gene_mutation"));

				return std::make_shared< function_opt_domain >(fdef, function_opt_domain::Goal::Minimise, gene_mut_fn);
			}

		}
	}
}

