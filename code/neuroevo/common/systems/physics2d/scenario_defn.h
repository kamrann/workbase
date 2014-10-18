// scenario_defn.h

#ifndef __NE_PHYS2D_SCENARIO_DEFN_H
#define __NE_PHYS2D_SCENARIO_DEFN_H

#include "system_sim/system_state_values.h"

#include "params/param_fwd.h"

#include <string>
#include <memory>


namespace sys {
	namespace phys2d {

		class scenario;

		class scenario_defn
		{
		public:
			virtual std::string get_name() const = 0;

			virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const = 0;

			virtual state_value_id_list get_state_values(prm::param_accessor acc) const = 0;

			virtual std::unique_ptr< scenario > create_scenario(prm::param_accessor acc) const = 0;
		};

		typedef std::unique_ptr< scenario_defn > scenario_defn_ptr;

	}
}


#endif

