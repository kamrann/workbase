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

			virtual ddl::defn_node get_defn(ddl::specifier& spc) override;

			virtual state_value_id_list get_state_values(ddl::navigator nav) const override;

			virtual std::unique_ptr< scenario > create_scenario(ddl::navigator nav) const override;

		protected:
			struct spec_data
			{
				double expanse;
				double incline;
				double friction;
				double gravity;

				size_t ball_count;
			};

			friend class ground_scenario;
		};

	}
}


#endif

