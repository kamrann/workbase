// rosenbrock.h

#ifndef __WB_GA_FUNCTION_OPT_ROSENBROCK_H
#define __WB_GA_FUNCTION_OPT_ROSENBROCK_H

#include "function_opt_domain.h"


namespace ga {
	namespace domain {
		namespace fnopt {
			namespace functions {

				class rosenbrock_defn
				{
				public:
					void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const;
					function_opt_domain::function_defn generate(prm::param_accessor acc) const;

					void update_schema_provider(prm::schema::schema_provider_map_handle, prm::qualified_path const&, int) const
					{}
				};

				function_opt_domain::function_defn rosenbrock(
					size_t n_dimensions,
					double lower = -50.0,
					double upper = 50.0,
					double a = 1.0,
					double b = 100.0
					);

			}
		}
	}
}


#endif


