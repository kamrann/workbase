// ski_jump_scenario.h

#ifndef __NE_PHYS2D_SKIJUMP_SCENARIO_H
#define __NE_PHYS2D_SKIJUMP_SCENARIO_H

#include "scenario.h"
#include "ski_jump_scenario_defn.h"

#include <Box2D/Box2D.h>

#include <boost/optional.hpp>


namespace sys {
	namespace phys2d {

		class ski_jump_scenario:
			public scenario
		{
		protected:
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const override;

		public:
			ski_jump_scenario(ski_jump_scenario_defn::spec_data spec);
			virtual bool create(phys2d_system const* sys) override;

			virtual void on_contact(b2Contact* contact, b2dc::ContactType ctype) override;
			virtual bool is_complete() const override;

		protected:
			bool on_platform() const;
			double platform_position() const;
			bool on_takeoff() const;
			double takeoff_position() const;

		private:
			enum class ScenarioComponent {
				Terrain,
			};

			struct terrain_sensor_data
			{
				enum {
					Platform,
					Takeoff,
				} type;

				size_t index;
			};

			ski_jump_scenario_defn::spec_data m_spec;

			b2Body* m_ground;

			b2Vec2 m_platform_line[2];
			b2Vec2 m_takeoff_line[2];

			std::vector< int > m_platform_sensors;
			std::vector< int > m_takeoff_sensors;

			double m_landing_x_start;
			boost::optional< double > m_landing_contact_min_x;
		};

	}
}


#endif

