// ga_params.h

#ifndef __WB_GA_PARAMS_H
#define __WB_GA_PARAMS_H

#include "genetic_algorithm/genalg.h"

//#include "params/param_fwd.h"
//#include "params/dynamic_enum_schema.h"
#include "ddl/ddl.h"
#include "ddl/components/enum_choice.h"


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

		ddl::defn_node get_defn_for_ga(ddl::specifier& spc);
		ddl::defn_node get_defn(ddl::specifier& spc);

		std::shared_ptr< i_problem_domain > generate_domain(ddl::navigator nav);
		std::unique_ptr< genalg > generate_alg(ddl::navigator nav, std::shared_ptr< i_problem_domain > domain);


		ddl::enum_choice< rate_fn_t > rate_fn_defn;	// Used for both cx and mut rates
		ddl::enum_choice< std::shared_ptr< i_problem_domain > > domain_defn;
	};

}


#endif


