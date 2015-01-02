// simple_timer_domain.h

#ifndef __WB_GA_SIMPLE_TIMER_DOMAIN_H
#define __WB_GA_SIMPLE_TIMER_DOMAIN_H

#include "genetic_algorithm/problem_domain.h"
#include "genetic_algorithm/objective_value.h"
#include "genetic_algorithm/operators.h"
#include "genetic_algorithm/fixed_real_genome.h"
#include "genetic_algorithm/real_valued_mutation.h"
#include "genetic_algorithm/real_valued_diversity.h"
#include "genetic_algorithm/genetic_population.h"

#include "neuralnet/implementations/mlf/fc_rnn.h"
#include "neuralnet/interface/input.h"
#include "neuralnet/interface/output.h"

#include "ddl/ddl.h"
#include "ddl/components/enum_choice.h"

#include <string>


namespace ga {
	namespace domain {
		namespace timer {

			typedef ga::gene_mutation< double > gene_mut_fn_t;

			class simple_timer_domain:
				public ga::i_problem_domain
			{
			public:
				enum LayerCounts {
					Input = 2,
					Hidden = 2,
					Output = 1,
				};

			public:
				simple_timer_domain(gene_mut_fn_t gene_mut_fn):
					gene_mut_fn_(gene_mut_fn)
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
						std::uniform_real_distribution< double > dist{ -1.0, 1.0 };	// TODO: range?
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
						gene_mut_fn_.update(ctx);
					};

					ga::basic_real_mutation< genome_t::gene_t > mut_fn{
						/*			std::bind(
									&function_opt_domain_defn::gene_mut_fn_t::op,
									&m_gene_mut_fn
									)
									*/
						[this](genome_t::gene_t& g, ga::rgen_t& rgen)
						{
							gene_mut_fn_.op(g, rgen);
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
					return true;
				}

				virtual std::vector< ga::objective_value > evaluate_genome(ga::genome const& gn, ga::rgen_t& rgen, size_t trials) const override
				{
					auto nn = decode(gn.as< genome_t >());

					size_t const TotalTimesteps = 500;
					double const TimerStartRate = 0.05;

					size_t times_started = 0;
					double cumulative_error = 0.0;

					boost::optional< size_t > timer;
					nnet::input inputs(nnet::input::input_list_t{ 0.0, 0.5 });
					for(size_t t = 0; t < TotalTimesteps; ++t)
					{
						// Was timer still active?
						double const expected_result = timer ? 1.0 : 0.0;

						if(timer)
						{
							--*timer;
							if(*timer == 0)
							{
								timer = boost::none;
							}
						}

						// NOTE: Currently possible for a new timer to be started at the same timestep as a 
						// previous one ended.

						inputs[0] = 0.0;
						if(!timer)
						{
							std::exponential_distribution<> dist(TimerStartRate);
							auto next_event = dist(rgen);
							if(next_event < 1.0)	// this timestep?
							{
								auto dur_dist = std::uniform_int_distribution < > { 1, 10 };
								auto duration = dur_dist(rgen);
								timer = duration;
								inputs[0] = 1.0;
								inputs[1] = 0.1 * duration;
								++times_started;
							}
						}

						auto nn_out = nn->run(inputs);
						cumulative_error += std::abs(nn_out[0] - expected_result);
					}

					return{ -cumulative_error };
				}

				virtual diversity_t population_genetic_diversity(ga::genetic_population const& pop) const override
				{
					return fixed_real_genome_population_genetic_diversity(pop, genome_length());
				}

				virtual void output_individual(ga::genome const& gn, std::ostream& os) const
				{
//					auto idv = decode(gn.as< genome_t >());
					os << "[ ";
					for(auto const& v : gn.as< genome_t >())
					{
						os << v << " ";
					}
					os << "]";
				}

			private:
				inline size_t genome_length() const
				{
					// TODO: hack
					nnet::fc_rnn nn;
					nn.create(LayerCounts::Input, LayerCounts::Hidden, LayerCounts::Output);
					return nn.num_connections();
				}

				inline std::shared_ptr< nnet::i_neuralnet > decode(genome_t const& gn) const
				{
					auto len = genome_length();
					auto rnn = std::make_shared< nnet::fc_rnn >();
					rnn->create(LayerCounts::Input, LayerCounts::Hidden, LayerCounts::Output);
					rnn->set_activation_function_hidden(nnet::activation_function{ nnet::ActivationFnType::SigmoidSymmetric });
					rnn->set_activation_function_output(nnet::activation_function{ nnet::ActivationFnType::Threshold });
					rnn->load_weights({ std::begin(gn), std::end(gn) });
					return rnn;
				}

			private:
				gene_mut_fn_t gene_mut_fn_;
			};


			class simple_timer_domain_defn
			{
			public:
				ddl::defn_node get_defn(ddl::specifier& spc);
				std::shared_ptr< ga::i_problem_domain > generate(ddl::navigator nav) const;

				ddl::enum_choice< gene_mut_fn_t > gene_mutation_defn_;

				simple_timer_domain_defn();
			};

		}
	}
}



#endif


