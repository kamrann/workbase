// ev_controller_genetic_mapping.h

#ifndef __WB_EV_CONTROLLER_GENETIC_MAPPING_H
#define __WB_EV_CONTROLLER_GENETIC_MAPPING_H

#include "system_sim/system_sim_fwd.h"

#include "genetic_algorithm/genalg_common.h"
#include "genetic_algorithm/genome.h"
#include "genetic_algorithm/genetic_population.h"
#include "genetic_algorithm/operators.h"


namespace sys {
	namespace ev {

		class i_genetic_mapping
		{
		public:
			typedef controller_ptr phenome_t;

			virtual ga::genome create_random_genome(ga::rgen_t& rgen) const = 0;
			virtual ga::crossover_fn_t crossover_op() const = 0;
			virtual ga::mutation mutation_op() const = 0;
			virtual bool rectify_genome(ga::genome& gn) const = 0;
			virtual ga::diversity_t population_genetic_diversity(ga::genetic_population const& pop) const = 0;
			// TODO: Temp
			virtual void output_individual(ga::genome const& gn, std::ostream& os) const = 0;

			virtual phenome_t decode(ga::genome const& gn) const = 0;
			virtual ga::genome load_genome_from_binary(std::vector< unsigned char > const& bytes) const = 0;

		public:
			~i_genetic_mapping() noexcept {}
		};

	}
}



#endif


