// contact_sensor_cmp.h

#ifndef __NE_PHYS2D_CONTACT_SENSOR_CMP_H
#define __NE_PHYS2D_CONTACT_SENSOR_CMP_H

#include "../sensors/basic_contact_sensor.h"

#include "system_sim/system_state_values.h"


namespace sys {
	namespace phys2d {

		class contact_sensor_cmp
		{
		public:
			static std::shared_ptr< contact_sensor_cmp > create(
				b2Body* body
				, b2Shape* shape
				, collision_filter filter = {}
				);

			static state_value_id_list get_state_values();

			size_t initialize_state_value_bindings_(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const;

		public:	// should be private but can't work with make_shared
			contact_sensor_cmp(std::shared_ptr< basic_contact_sensor > sen);

		protected:
			std::shared_ptr< basic_contact_sensor > sensor_;
		};

	}
}


#endif

