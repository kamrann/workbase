// rosenbrock.h

#ifndef __WB_GA_FUNCTION_OPT_ROSENBROCK_H
#define __WB_GA_FUNCTION_OPT_ROSENBROCK_H

#include "function_opt_domain.h"
#include "ddl/ddl.h"


namespace ga {
	namespace domain {
		namespace fnopt {
			namespace functions {

				struct rosenbrock_schema
				{
					ddl::defn_node get_defn(ddl::specifier& spc);
					function_opt_domain::function_defn generate(ddl::navigator nav) const;
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


