// evo_params_tbl.h

#ifndef __GADB_EVO_PARAMS_H
#define __GADB_EVO_PARAMS_H

#include "table_traits.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class evo_run;

class evo_params
{
public:
	dbo::ptr< evo_run >		run;
	int						initial_pop_size;
	int						num_generations;
	int						trials_per_generation;
	int						random_seed;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, run, "evo_run", dbo::NotNull | dbo::OnDeleteCascade);
		dbo::field(a, initial_pop_size, "initial_pop_size");
		dbo::field(a, num_generations, "num_generations");
		dbo::field(a, trials_per_generation, "trials_per_generation");
		dbo::field(a, random_seed, "random_seed");

//		dbo::belongsTo(a, run, "evo_params__evo_run");
	}
};


#endif


