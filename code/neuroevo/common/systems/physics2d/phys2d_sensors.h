// phys2d_sensors.h

#ifndef __NE_PHYS2D_SENSORS_H
#define __NE_PHYS2D_SENSORS_H

#include "phys2d_contacts.h"

#include <box2d/box2d.h>

#include <boost/any.hpp>


namespace sys {
	namespace phys2d {

		enum class SensorType {
			Directional,
			Fan,

			Undefined,
		};

		struct phys_sensor_defn
		{
			SensorType type;
			b2Body* body;
			b2Vec2 pos;
			float orientation;
			float range;

			boost::any type_specific;	// TODO: Variant

			phys_sensor_defn(
				SensorType type_ = SensorType::Undefined
				):
				type(type_),
				body(nullptr),
				pos(0, 0),
				orientation(0),
				range(0)
			{}
		};

		struct directional_sensor_data
		{};

		struct fan_sensor_data
		{
			float field_of_view;
		};


		struct phys_sensor
		{
			typedef double sensor_value_t;

			virtual sensor_value_t read() const = 0;
			//virtual void draw(Wt::WPainter& painter, b2Vec2 const& indicator_offset) const = 0;
		};

		struct contact_based_sensor: public phys_sensor
		{
			virtual void on_contact(ContactType type) = 0;
		};


		std::unique_ptr< phys_sensor > create_sensor(phys_sensor_defn const& defn);

	}
}


#endif

