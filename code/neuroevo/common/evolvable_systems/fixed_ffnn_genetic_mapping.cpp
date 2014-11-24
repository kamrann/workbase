// fixed_ffnn_genetic_mapping.cpp

#include "fixed_ffnn_genetic_mapping.h"
#include "controllers/nn_controller.h"

#include "genetic_algorithm/real_valued_diversity.h"


namespace sys {
	namespace ev {

		fixed_ffnn_genetic_mapping::fixed_ffnn_genetic_mapping(state_value_id_list inputs, nnet::mlp::layer_counts_t nn_layers, gene_mut_fn_t mut_fn):
			inputs_(inputs),
			layer_counts_(nn_layers),
			gene_mut_fn_(mut_fn)
		{
		}

		size_t fixed_ffnn_genetic_mapping::genome_length() const
		{
			// TODO: this should be a function in neuralnet library
			size_t weight_count = 0;
			auto num_layers = layer_counts_.size();
			for(size_t i = 1; i < num_layers; ++i)
			{
				size_t source_count = layer_counts_[i - 1] + 1;	// assuming 1 bias per layer (excl output)
				weight_count += source_count * layer_counts_[i];
			}
			return weight_count;
		}

		ga::genome fixed_ffnn_genetic_mapping::create_random_genome(ga::rgen_t& rgen) const
		{
			auto gn = std::make_unique< genome_t >();
			auto const len = genome_length();
			gn->resize(len);
			for(size_t i = 0; i < len; ++i)
			{
				std::uniform_real_distribution< genome_t::gene_t > dist{ /* TODO: range */ -1.0, 1.0 };
				(*gn)[i] = dist(rgen);
			}
			return std::move(gn);
		}

		ga::crossover_fn_t fixed_ffnn_genetic_mapping::crossover_op() const
		{
			ga::basic_array_crossover< genome_t > _crossover{ genome_length() };
			return [_crossover](ga::genome const& p1, ga::genome const& p2, ga::rgen_t& rgen) mutable
			{
				return std::make_unique< genome_t >(_crossover(p1.as< genome_t >(), p2.as< genome_t >(), rgen));
			};
		}

		ga::mutation fixed_ffnn_genetic_mapping::mutation_op() const
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

		bool fixed_ffnn_genetic_mapping::rectify_genome(ga::genome& gn) const
		{
			// TODO: bounded weights? domain specific, such as disabling equal and opposite activated thrusters?
			return true;
		}

		ga::diversity_t fixed_ffnn_genetic_mapping::population_genetic_diversity(ga::genetic_population const& pop) const
		{
			return fixed_real_genome_population_genetic_diversity(pop, genome_length());
		}

		void fixed_ffnn_genetic_mapping::output_individual(ga::genome const& gn, std::ostream& os) const
		{
			auto weight_gn = gn.as< genome_t >();
			os << "[ ";
			for(auto const& v : weight_gn)
			{
				os << v << " ";
			}
			os << "]";
		}

		fixed_ffnn_genetic_mapping::phenome_t fixed_ffnn_genetic_mapping::decode(ga::genome const& gn) const
		{
			auto weight_gn = gn.as< genome_t >();

			std::unique_ptr< nnet::mlp > nn = std::make_unique< nnet::mlp >();
			nn->create(layer_counts_);
			
			// TODO: from params
			for(size_t layer = 1; layer < layer_counts_.size(); ++layer)
			{
				nn->set_activation_function_layer(layer, nnet::activation_function{ nnet::ActivationFnType::SigmoidSymmetric });
			}

			nnet::weight_array_t weights{ std::begin(weight_gn), std::end(weight_gn) };
			nn->load_weights(weights);

			// TODO: If easier, could have the domain pass a system pointer into this method, which we
			// can use to create input value bindings here and pass to controller, so it doesn't need to store 
			// string versions
			return std::make_unique< nn_controller >(std::move(nn), inputs_);
		}

		ga::genome fixed_ffnn_genetic_mapping::load_genome_from_binary(std::vector< unsigned char > const& bytes) const
		{
			auto gn = genome_t::from_binary(bytes);
			if(gn.size() != genome_length())
			{
				throw std::runtime_error("genome length mismatch");
			}

			return std::make_unique< genome_t >(gn);
		}

	}
}



