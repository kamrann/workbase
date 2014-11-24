// ga_params.h

#ifndef __WB_GA_PARAMS_H
#define __WB_GA_PARAMS_H

#include "genetic_algorithm/genalg.h"

#include "params/param_fwd.h"
#include "params/dynamic_enum_schema.h"


namespace ga {

	// TODO: Not here
	namespace domain {
		enum Components {
			CrossoverOp,
			MutationOp,
//			DomainCore
		};
	}
	//

	class i_problem_domain;

	struct ga_defn
	{
		ga_defn();

		std::string update_schema_provider_for_ga(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);

		std::shared_ptr< i_problem_domain > generate_domain(prm::param_accessor acc);
		std::unique_ptr< genalg > generate_alg(prm::param_accessor acc, std::shared_ptr< i_problem_domain > domain);


		prm::dynamic_enum_schema< rate_fn_t > rate_fn_defn;	// Used for both cx and mut rates
		prm::dynamic_enum_schema< std::shared_ptr< i_problem_domain > > domain_defn;
	};

}


#endif


