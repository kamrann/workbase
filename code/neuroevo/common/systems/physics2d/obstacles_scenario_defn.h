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
			virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;

			virtual state_value_id_list get_state_values(prm::param_accessor acc) const override;

			virtual std::unique_ptr< scenario > create_scenario(prm::param_accessor acc) const override;
		};

	}
}


#endif

