// phys2d_object.h

#ifndef __NE_RTP_PHYS2D_OBJECT_H
#define __NE_RTP_PHYS2D_OBJECT_H

#include "phys2d_contacts.h"

#include "system_sim/system_state_values.h"

#include "util/bimap.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class object
		{
		public:			
			object(phys2d_system* system = nullptr);

		public:
			virtual void translate(b2Vec2 const& vec) = 0;
			virtual void rotate(float angle) = 0;
			virtual void set_linear_velocity(b2Vec2 const& vel) = 0;
			virtual void set_angular_velocity(float vel) = 0;

			virtual b2Vec2 get_position() const = 0;
			virtual b2Vec2 get_linear_velocity() const = 0;
			virtual float get_kinetic_energy() const = 0;

			virtual void on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type)
			{}
			virtual void on_collision(b2Fixture* fixA, b2Fixture* fixB, double approach_speed)
			{}

			static state_value_id_list get_state_values();

			enum class StateValue {
				PositionX,
				PositionY,
				LinVelX,
				LinVelY,
				KE,
			};

			static bimap< StateValue, std::string > const s_sv_names;

			phys2d_system* m_sys;
		};

	}
}



#endif

