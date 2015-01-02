// obstacles_scenario_defn.h

#ifndef __NE_PHYS2D_OBSTACLES_SCENARIO_DEFN_H
#define __NE_PHYS2D_OBSTACLES_SCENARIO_DEFN_H

#include "scenario_defn.h"


namespace sys {
	namespace phys2d {

		class obstacles_scenario_defn:
			public scenario_defn
		{
		public:
			virtual ddl::defn_node get_defn(ddl::specifier& spc) override;

			virtual state_value_id_list get_state_values(ddl::navigator nav) const override;

			virtual std::unique_ptr< scenario > create_scenario(ddl::navigator nav) const override;
		};

	}
}


#endif

