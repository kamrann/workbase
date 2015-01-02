// scenario_defn.h

#ifndef __NE_PHYS2D_SCENARIO_DEFN_H
#define __NE_PHYS2D_SCENARIO_DEFN_H

#include "system_sim/system_state_values.h"

#include "ddl/ddl.h"

#include <string>
#include <memory>


namespace sys {
	namespace phys2d {

		class scenario;

		class scenario_defn
		{
		public:
			virtual std::string get_name() const = 0;

			virtual ddl::defn_node get_defn(ddl::specifier& spc) = 0;

			virtual state_value_id_list get_state_values(ddl::navigator nav) const = 0;

			virtual std::unique_ptr< scenario > create_scenario(ddl::navigator nav) const = 0;
		};

		typedef std::unique_ptr< scenario_defn > scenario_defn_ptr;

	}
}


#endif

