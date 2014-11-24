// problem_domain.h
/*
An interface for the problem domain for which the genetic algorithm will search for solutions.
*/

#ifndef __WB_GA_PROBLEM_DOMAIN_H
#define __WB_GA_PROBLEM_DOMAIN_H

#include "genalg_common.h"
#include "objective_value.h"
#include "operators.h"

#include <iosfwd>


namespace ga {

	class genome;
	class genetic_population;

	class i_problem_domain
	{
	public:
		virtual objective_value::type objective_value_type() const = 0;

		virtual genome create_random_genome(rgen_t& rgen) const = 0;
		virtual crossover_fn_t crossover_op() const = 0;
		virtual mutation mutation_op() const = 0;
		virtual bool rectify_genome(ga::genome& gn) const = 0;
		virtual objective_value evaluate_genome(genome const& gn) const = 0;
		virtual diversity_t population_genetic_diversity(genetic_population const& pop) const = 0;

		// TODO: Temp
		virtual void output_individual(genome const& gn, std::ostream& os) const = 0;

	public:
		virtual ~i_problem_domain() {}
	};

}


#endif


