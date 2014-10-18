// ground_scenario_defn.h

#ifndef __NE_PHYS2D_GROUND_SCENARIO_DEFN_H
#define __NE_PHYS2D_GROUND_SCENARIO_DEFN_H

#include "scenario_defn.h"


namespace sys {
	namespace phys2d {

		class ground_scenario_defn:
			public scenario_defn
		{
		public:
			virtual std::string get_name() const override;

			virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;

			virtual state_value_id_list get_state_values(prm::param_accessor acc) const override;

			virtual std::unique_ptr< scenario > create_scenario(prm::param_accessor acc) const override;

		protected:
			struct spec_data
			{
				double expanse;
			};

			friend class ground_scenario;
		};

	}
}


#endif

