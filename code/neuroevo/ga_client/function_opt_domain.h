// function_opt_domain.h

#ifndef __WB_GA_FUNCTION_OPT_DOMAIN_H
#define __WB_GA_FUNCTION_OPT_DOMAIN_H

#include "genetic_algorithm/problem_domain.h"
#include "genetic_algorithm/objective_value.h"
#include "genetic_algorithm/operators.h"
#include "genetic_algorithm/fixed_real_genome.h"
#include "genetic_algorithm/real_valued_mutation.h"
#include "genetic_algorithm/real_valued_diversity.h"
#include "genetic_algorithm/genetic_population.h"

#include "params/param_fwd.h"
#include "params/dynamic_enum_schema.h"

#include <string>


namespace ga {
	namespace domain {
		namespace fnopt {

			typedef ga::gene_mutation< double > gene_mut_fn_t;

			class function_opt_domain:
				public ga::i_problem_domain
			{
			public:
				typedef double value_t;

				struct parameter_defn
				{
					value_t lower_bound;
					value_t upper_bound;

					parameter_defn():
						lower_bound(std::numeric_limits< value_t >::min()),
						upper_bound(std::numeric_limits< value_t >::max())
					{}
				};

				typedef std::vector< value_t > parameter_list;

				struct function_defn
				{
					std::vector< parameter_defn > parameters;

					std::function< value_t(parameter_list const& params) > eval;
				};

				enum class Goal {
					Minimise,
					Maximise,
				};

			public:
				function_opt_domain(function_defn defn, Goal goal, gene_mut_fn_t gene_mut_fn):
					m_defn(defn),
					m_goal(goal),
					m_gene_mut_fn(gene_mut_fn)
				{}

				typedef ga::fixed_real_genome genome_t;

			public:
				virtual objective_value::type objective_value_type() const override
				{
					return objective_value::type::Real;
				}

				virtual ga::genome create_random_genome(ga::rgen_t& rgen) const override
				{
					auto gn = std::make_unique< genome_t >();
					auto const len = genome_length();
					gn->resize(len);
					for(size_t i = 0; i < len; ++i)
					{
						std::uniform_real_distribution< value_t > dist{ m_defn.parameters[i].lower_bound, m_defn.parameters[i].upper_bound };
						(*gn)[i] = dist(rgen);
					}
					return std::move(gn);
				}

				virtual ga::crossover_fn_t crossover_op() const override
				{
					ga::basic_array_crossover< genome_t > _crossover{ genome_length() };
					return [_crossover](ga::genome const& p1, ga::genome const& p2, ga::rgen_t& rgen) mutable
					{
						return std::make_unique< genome_t >(_crossover(p1.as< genome_t >(), p2.as< genome_t >(), rgen));
					};
				}

				virtual ga::mutation mutation_op() const override
				{
					// TODO: This is a total fucking mess
					ga::mutation mut;

					// TODO: why bind doesnt work?
					mut.update = //std::bind(&function_opt_domain_defn::gene_mut_fn_t::update, &m_gene_mut_fn);
						[this](ga::op_update_context const& ctx)
					{
						m_gene_mut_fn.update(ctx);
					};

					ga::basic_real_mutation< genome_t::gene_t > mut_fn{
						/*			std::bind(
									&function_opt_domain_defn::gene_mut_fn_t::op,
									&m_gene_mut_fn
									)
									*/
						[this](genome_t::gene_t& g, ga::rgen_t& rgen)
						{
							m_gene_mut_fn.op(g, rgen);
						}
					};

					mut.op = [mut_fn](ga::genome& gn, double const rate, ga::rgen_t& rgen) mutable
					{
						mut_fn(gn.as< genome_t >(), rate, rgen);
					};

					return mut;
				}

				virtual bool rectify_genome(ga::genome& gn) const override
				{
					auto const len = genome_length();
					auto& genes = gn.as< genome_t >();
					for(size_t i = 0; i < len; ++i)
					{
						auto& g = genes[i];
						auto const& pd = m_defn.parameters[i];
						g = std::max(g, pd.lower_bound);
						g = std::min(g, pd.upper_bound);
					}
					return true;
				}

				virtual ga::objective_value evaluate_genome(ga::genome const& gn) const override
				{
					auto params = decode(gn.as< genome_t >());
					auto output = m_defn.eval(params);
					return m_goal == Goal::Maximise ? output : -output;
				}

				virtual diversity_t population_genetic_diversity(ga::genetic_population const& pop) const override
				{
					return fixed_real_genome_population_genetic_diversity(pop, genome_length());
				}

				virtual void output_individual(ga::genome const& gn, std::ostream& os) const
				{
					auto idv = decode(gn.as< genome_t >());
					os << "[ ";
					for(auto const& v : idv)
					{
						os << v << " ";
					}
					os << "]";
				}

			private:
				inline size_t genome_length() const
				{
					return m_defn.parameters.size();
				}

				inline parameter_list decode(genome_t const& gn) const
				{
					auto len = genome_length();
					parameter_list result(len);
					for(size_t i = 0; i < len; ++i)
					{
						result[i] = gn[i];
					}
					return result;
				}

			private:
				function_defn m_defn;
				Goal m_goal;
				gene_mut_fn_t m_gene_mut_fn;
			};


			class function_opt_domain_defn
			{
			public:
				void update_schema_provider_for_crossover_op(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);// const;
				void update_schema_provider_for_mutation_op(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);// const;

				void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);// const;
				void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, int component);// const;
				std::shared_ptr< ga::i_problem_domain > generate(prm::param_accessor acc) const;

				prm::dynamic_enum_schema< gene_mut_fn_t > gene_mutation_defn;
				prm::dynamic_enum_schema< function_opt_domain::function_defn > func_defn;

				function_opt_domain_defn();
			};

		}
	}
}



#endif


