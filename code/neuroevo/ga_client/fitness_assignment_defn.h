// fitness_assignment_defn.h

#ifndef __WB_GA_FITNESS_ASSIGNMENT_DEFN_H
#define __WB_GA_FITNESS_ASSIGNMENT_DEFN_H

#include "params/param_fwd.h"


namespace ga {
	/*
	TODO: This should provide options to choose pareto ranking method, only if the domain defn is configured to
	generate pareto objective values.
	probably need to allow dynamic_enum_schema to be templated on a virtual interface, such that each registered
	options implements that interface, and the dynamic_enum_schema has a method returning a pointer to that
	interface for the currently selected option. then in this case, that interface can have a method such as
	is_pareto_objective().

	struct fitness_assignment_defn
	{
		fitness_assignment_defn();

		std::string update_schema_provider_for_ga(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);

		std::shared_ptr< i_problem_domain > generate_domain(prm::param_accessor acc);
		std::unique_ptr< genalg > generate_alg(prm::param_accessor acc, std::shared_ptr< i_problem_domain > domain);


		prm::dynamic_enum_schema< rate_fn_t > rate_fn_defn;	// Used for both cx and mut rates
		prm::dynamic_enum_schema< std::shared_ptr< i_problem_domain > > domain_defn;
	};
	*/

}


#endif


