// system_simulation_interface.h

#ifndef __SYSTEM_SIMULATION_INTERFACE_H
#define __SYSTEM_SIMULATION_INTERFACE_H

/*
define type: solution_result

define method: void register_solution_result(solution_result);	// TODO: Parameter identifying solution instance.
	// Also, ideally it would be possible not only for multiple solution instances to partake in a system simulation,
	// but even multiple different types of solution (to different problems, eg. predator & prey, or one ship aiming to
	// reach a given location, another ship aiming to destroy the first).
	
define method: void update();	// TODO: Time parameter?

// TODO: Something along lines of: get_state()

// TODO: Will need something like: bool needs_to_process(solution_id) to query if a given solution instance needs to process and register it's result at this stage
// So for realtime systems generally every instance will need to process at every stage, but for a turn based system (such as noughts and crosses),
// this can be alternated.
// OR, should all instances just be registered once at the start, and then update() invokes all necessary solutions and processes results...?
*/


/*
Perhaps instead of ProblemDomain, we want something like Scenario, which defines the initial state of the SystemSimulation,
a subset of SystemSimulation.State as the inputs to the solution, maybe along with a specification of which types of solutions
are to be evolved (eg. Player X, Player O, both, or Generic Player), how many of each type are present in a single system instance
(eg. we could evolve X vs Y, or separately vs a random player), and the scope of the fitness trial (eg. play to game completion, or
just single move then evaluate).
*/


#include "noughts_and_crosses/noughts_and_crosses_system.h"
#include "ship_and_thrusters/ship_and_thrusters_system.h"

#include <boost/mpl/set.hpp>

struct systems
{
	typedef boost::mpl::set<
		noughts_and_crosses_system< 2, 3 >,
		noughts_and_crosses_system< 3, 3 >,
		ship_and_thrusters_system< WorldDimensionality::dim2D >
	> system_set_t;
};


#endif


