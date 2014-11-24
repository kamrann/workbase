// real_valued_diversity.h

#ifndef __WB_GA_REAL_VALUED_DIVERSITY_H
#define __WB_GA_REAL_VALUED_DIVERSITY_H

#include "genetic_population.h"
#include "fixed_real_genome.h"


namespace ga {

	// This implementation for populations where all genomes are of same length
	inline diversity_t fixed_real_genome_population_genetic_diversity(ga::genetic_population const& pop, size_t const genome_len)
	{
		// See Google: "measure+diversity+real+valued+chromosomes" (google books result)

		typedef fixed_real_genome genome_t;

		std::vector< genome_t::gene_t > avg_gene_vals(genome_len, 0.0);
		std::vector< genome_t::gene_t > avg_gene_squared_vals(genome_len, 0.0);
		auto const pop_size = pop.size();
		for(size_t g = 0; g < genome_len; ++g)
		{
			double sum = 0.0;
			double sum_of_squares = 0.0;
			for(size_t n = 0; n < pop_size; ++n)
			{
				auto const& gn = pop[n].gn.as< genome_t >();
				sum += gn[g];
				sum_of_squares += gn[g] * gn[g];
			}

			avg_gene_vals[g] = sum / pop_size;
			avg_gene_squared_vals[g] = sum_of_squares / pop_size;
		}

		diversity_t diversity = 0.0;
		for(size_t g = 0; g < genome_len; ++g)
		{
			diversity += avg_gene_squared_vals[g] - avg_gene_vals[g] * avg_gene_vals[g];
		}

		// When diversity is essentially 0, precision error can result in very small negative value, which
		// when passed to sqrt generates NaN. So clamp to 0+ first.
		diversity = std::max(diversity, 0.0);
		diversity = std::sqrt(diversity) / genome_len;
		return diversity;
	}

}



#endif


