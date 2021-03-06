// ackley.h

#ifndef __WB_GA_FUNCTION_OPT_ACKLEY_H
#define __WB_GA_FUNCTION_OPT_ACKLEY_H

#include "function_opt_domain.h"
#include "ddl/ddl.h"


namespace ga {
	namespace domain {
		namespace fnopt {
			namespace functions {

				struct ackley_schema
				{
					ddl::defn_node get_defn(ddl::specifier& spc);
					function_opt_domain::function_defn generate(ddl::navigator nav) const;
				};

				function_opt_domain::function_defn ackley(
					size_t n_dimensions,
					double lower = -20.0,
					double upper = 20.0,
					double a = 20.0,
					double b = 0.2,
					double c = 2 * M_PI
					);

			}
		}
	}
}


#endif


