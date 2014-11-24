// fitness_assignment.h

#ifndef __WB_GA_FITNESS_ASSIGNMENT_H
#define __WB_GA_FITNESS_ASSIGNMENT_H

#include <functional>


namespace ga {

	class genetic_population;

	typedef std::function< void(genetic_population&) > fitness_assignment_fn_t;

	void direct_fitness_assignment(genetic_population& pop);
	void pareto_nondominated_ranking_fitness_assignment(genetic_population& pop);

	// TODO: Real rank based, pareto rank based (various ranking methods)

}


#endif


