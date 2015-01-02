// evolved_system_domain.h

#ifndef __WB_EV_SYS_DOMAIN_H
#define __WB_EV_SYS_DOMAIN_H

#include "ev_controller_genetic_mapping.h"
#include "objective.h"

#include "system_sim/system_sim_fwd.h"

#include "genetic_algorithm/problem_domain.h"
#include "genetic_algorithm/objective_value.h"
#include "genetic_algorithm/operators.h"
#include "genetic_algorithm/fixed_real_genome.h"
#include "genetic_algorithm/real_valued_mutation.h"
#include "genetic_algorithm/real_valued_diversity.h"
#include "genetic_algorithm/genetic_population.h"


namespace sys {
	namespace ev {

		class evolved_system_domain:
			public ga::i_problem_domain
		{
		public:
			evolved_system_domain(
				system_ptr sys,
				std::unique_ptr< i_genetic_mapping > mapping,
				std::unique_ptr< objective > objective
				);

		public:
			virtual ga::objective_value::type objective_value_type() const override;
			virtual ga::genome create_random_genome(ga::rgen_t& rgen) const override;
			virtual ga::crossover_fn_t crossover_op() const override;
			virtual ga::mutation mutation_op() const override;
			virtual bool rectify_genome(ga::genome& gn) const override;
			virtual std::vector< ga::objective_value > evaluate_genome(ga::genome const& gn, ga::rgen_t& rgen, size_t trials) const override;
			virtual ga::diversity_t population_genetic_diversity(ga::genetic_population const& pop) const override;

			// TODO: Temp
			virtual void output_individual(ga::genome const& gn, std::ostream& os) const override;

		private:
			inline size_t genome_length() const;

		public:
			system_ptr system_;
			std::unique_ptr< i_genetic_mapping > mapping_;
			std::unique_ptr< objective > objective_;
		};

	}
}



#endif


