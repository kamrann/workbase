// ski_jump_scenario_defn.h

#ifndef __NE_PHYS2D_SKIJUMP_SCENARIO_DEFN_H
#define __NE_PHYS2D_SKIJUMP_SCENARIO_DEFN_H

#include "scenario_defn.h"


namespace sys {
	namespace phys2d {

		class ski_jump_scenario_defn:
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
				double inrun_degrees;
				double inrun_length;
				double transition1_radius;
				double takeoff_degrees;
				double takeoff_length;
				double jump_dropoff;
				double landing_length;
				double landing_drop;
				double landing_x1;
				double landing_x2;
				double wall_height;
			};

			friend class ski_jump_scenario;
		};

	}
}


#endif

