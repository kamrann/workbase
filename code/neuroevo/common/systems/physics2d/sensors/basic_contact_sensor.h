// basic_contact_sensor.h
/*
Simple sensor which tracks overlap contact with a single body.
Implementation is a bit of a hack, ignoring all but the first fixture contact.
Behaviour should be fine in cases where the sensor size is small relative to world bodies.
TODO: Will need to allow filter configuring.
*/

#ifndef __NE_PHYS2D_BASIC_CONTACT_SENSOR_H
#define __NE_PHYS2D_BASIC_CONTACT_SENSOR_H

#include "sensors.h"

#include <memory>


namespace sys {
	namespace phys2d {

		class basic_contact_sensor:
			public contact_based_sensor
		{
		public:
			static std::shared_ptr< basic_contact_sensor > create(
				b2Shape* shape
				, b2Body* body
				, collision_filter filter = {});

		public:
			basic_contact_sensor(b2Shape* shape, b2Body* body, collision_filter filter = {});

			inline bool is_contacting() const
			{
				return contact_count_ > 0;
			}

			inline b2Body* get_contact_body() const
			{
				return contact_body_;
			}

		protected:
			virtual void on_contact(ContactType type, b2Body* body) override;

		protected:
			b2Body* contact_body_;
			size_t contact_count_;
		};

	}
}


#endif

