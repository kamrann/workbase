// genalg.h

#ifndef __WB_GA_GENALG_H
#define __WB_GA_GENALG_H

#include "procreator.h"
#include "genetic_population.h"
#include "trials.h"
#include "fitness_assignment.h"


namespace ga {

	class i_problem_domain;

	/*
	Encapsulates all components of the genetic algorithm.
	*/
	class genalg
	{
	public:
		genalg(
			i_problem_domain const& domain
			);

		void set_initial_population_size(size_t sz);
		void set_crossover_rate_fn(crossover_rate_fn_t fn);
		void set_mutation_rate_fn(mutation_rate_fn_t fn);
		void set_trials_configuration(trials_cfg cfg);
		void set_fitness_assignment(fitness_assignment_fn_t fn);

	public:
		void reset(/*size_t initial_pop_size, */unsigned int rseed);
		void epoch();

		size_t generation() const;
		genetic_population const& population() const;
		i_problem_domain const& domain() const;

	private:
		size_t initial_pop_size_;
		genetic_population pop_;
		procreator proc_;
		i_problem_domain const& domain_;
		trials_cfg trials_;
		fitness_assignment_fn_t fitness_assign_;
		rgen_t rgen_;
	};

}


#endif


