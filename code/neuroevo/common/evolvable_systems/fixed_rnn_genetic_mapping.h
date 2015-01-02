// fixed_rnn_genetic_mapping.h

#ifndef __WB_EV_FIXED_RNN_GENETIC_MAPPING_H
#define __WB_EV_FIXED_RNN_GENETIC_MAPPING_H

#include "ev_controller_genetic_mapping.h"

#include "system_sim/system_state_values.h"
#include "neuralnet/implementations/mlf/fc_rnn.h"
#include "genetic_algorithm/fixed_real_genome.h"


namespace sys {
	namespace ev {

		class fixed_rnn_genetic_mapping:
			public i_genetic_mapping
		{
		public:
			typedef ga::fixed_real_genome genome_t;
			typedef ga::gene_mutation< genome_t::gene_t > gene_mut_fn_t;

		public:
			fixed_rnn_genetic_mapping(
				state_value_id_list inputs,
				nnet::fc_rnn::layer_counts_t nn_layers,
				nnet::activation_function hidden_af,
				nnet::activation_function output_af,
				gene_mut_fn_t mut_fn);

		public:
			virtual ga::genome create_random_genome(ga::rgen_t& rgen) const override;
			virtual ga::crossover_fn_t crossover_op() const override;
			virtual ga::mutation mutation_op() const override;
			virtual bool rectify_genome(ga::genome& gn) const override;
			virtual ga::diversity_t population_genetic_diversity(ga::genetic_population const& pop) const override;

			// TODO: Temp
			virtual void output_individual(ga::genome const& gn, std::ostream& os) const override;

			virtual phenome_t decode(ga::genome const& gn) const override;
			virtual ga::genome load_genome_from_binary(std::vector< unsigned char > const& bytes) const override;

		private:
			size_t genome_length() const;

		private:
			gene_mut_fn_t gene_mut_fn_;

			state_value_id_list inputs_;
			nnet::fc_rnn::layer_counts_t layer_counts_;
			nnet::activation_function hidden_af_;
			nnet::activation_function output_af_;
		};

	}
}



#endif


