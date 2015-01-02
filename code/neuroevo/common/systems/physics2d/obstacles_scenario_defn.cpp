// obstacles_scenario_defn.cpp

#include "obstacles_scenario_defn.h"
#include "obstacles_scenario.h"


namespace sys {
	namespace phys2d {

		ddl::defn_node obstacles_scenario_defn::get_defn(ddl::specifier& spc)
		{
			return{};
		}

		state_value_id_list obstacles_scenario_defn::get_state_values(ddl::navigator nav) const
		{
			return{};
		}

		std::unique_ptr< scenario > obstacles_scenario_defn::create_scenario(ddl::navigator nav) const
		{
			return{};
		}

	}
}


