// ground_scenario.h

#ifndef __NE_PHYS2D_GROUND_SCENARIO_H
#define __NE_PHYS2D_GROUND_SCENARIO_H

#include "scenario.h"
#include "ground_scenario_defn.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class ground_scenario:
			public scenario
		{
		protected:
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const override;

		public:
			ground_scenario(ground_scenario_defn::spec_data spec);
			virtual bool create(phys2d_system const* sys) override;

			virtual bool is_complete() const override;

		private:
			ground_scenario_defn::spec_data m_spec;

			b2Body* m_ground;
		};

	}
}


#endif

