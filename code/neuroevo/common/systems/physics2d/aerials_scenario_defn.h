// aerials_scenario_defn.h

#ifndef __NE_PHYS2D_AERIALS_SCENARIO_DEFN_H
#define __NE_PHYS2D_AERIALS_SCENARIO_DEFN_H

#include "scenario_defn.h"


namespace sys {
	namespace phys2d {

		class aerials_scenario_defn:
			public scenario_defn
		{
		public:
			virtual std::string get_name() const override;

			virtual ddl::defn_node get_defn(ddl::specifier& spc) override;

			virtual state_value_id_list get_state_values(ddl::navigator nav) const override;

			virtual std::unique_ptr< scenario > create_scenario(ddl::navigator nav) const override;

		protected:
			struct spec_data
			{
				double gravity;
				double surface_friction;

				double overall_scale;
				double platform_width;
				double slope_gradient;
				double slope_drop;
				double dip_depth;
				double ramp_gradient;
				double ramp_width;
				double landing_slope_drop;
				double landing_flat_width;
				double wall_height;
			};

			friend class aerials_scenario;
		};

	}
}


#endif

