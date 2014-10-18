// obstacles_scenario_defn.cpp

#include "obstacles_scenario_defn.h"
#include "obstacles_scenario.h"

#include "params/param_accessor.h"


namespace sys {
	namespace phys2d {

		std::string obstacles_scenario_defn::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			return{};
		}

		state_value_id_list obstacles_scenario_defn::get_state_values(prm::param_accessor acc) const
		{
			return{};
		}

		std::unique_ptr< scenario > obstacles_scenario_defn::create_scenario(prm::param_accessor acc) const
		{
			return{};
		}

	}
}


