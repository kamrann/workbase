// fitness_assignment.cpp

#include "fitness_assignment.h"
#include "genetic_population.h"


namespace ga {

	// Assumes real valued objective values, and simply assigns them to the fitness unchanged.
	void direct_fitness_assignment(genetic_population& pop)
	{
		for(auto& idv : pop)
		{
			idv.fitness = idv.obj_val.as_real();
		}
	}

	/*
	Ranks individuals based on pareto-dominance fronts.
	*/
	void pareto_nondominated_ranking_fitness_assignment(genetic_population& pop)
	{
		auto const pop_size = pop.size();
		std::vector< pareto< double > > pareto_vals(pop_size);
		for(size_t i = 0; i < pop_size; ++i)
		{
			pareto_vals[i] = pop[i].obj_val.as_pareto();
		}

		std::vector< size_t > ranks(pop_size);
		pareto< double >::calc_ranks(std::begin(pareto_vals), std::end(pareto_vals), std::begin(ranks));
		for(size_t i = 0; i < pop_size; ++i)
		{
			pop[i].fitness = -1.0 * ranks[i];
		}
	}

}




