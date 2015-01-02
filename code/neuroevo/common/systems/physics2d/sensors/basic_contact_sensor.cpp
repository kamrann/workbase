// basic_contact_sensor.cpp
/*
Simple sensor which tracks overlap contact with a single body.
Implementation is a bit of a hack, ignoring all but the first fixture contact.
Behaviour should be fine in cases where the sensor size is small relative to world bodies.
TODO: Will need to allow filter configuring.
*/

#include "basic_contact_sensor.h"
#include "../phys2d_entity_data.h"


namespace sys {
	namespace phys2d {

		std::shared_ptr< basic_contact_sensor > basic_contact_sensor::create(
			b2Shape* shape
			, b2Body* body
			, collision_filter filter)
		{
			return std::make_shared< basic_contact_sensor >(shape, body, filter);
		}

		// TODO: Allow body to be nullptr, in which case add fixture to a single world sensor body
		basic_contact_sensor::basic_contact_sensor(b2Shape* shape, b2Body* body, collision_filter filter):
			contact_body_{ nullptr }
			, contact_count_{ 0 }
		{
			b2FixtureDef fd;
			fd.isSensor = true;
			fd.shape = shape;
			fd.filter.groupIndex = filter.group_index;
			auto fix = body->CreateFixture(&fd);

			set_fixture_data(fix, entity_fix_data{
				entity_fix_data::Type::Sensor,
				entity_fix_data::val_t{ static_cast<contact_based_sensor*>(this) }
			});
		}

		void basic_contact_sensor::on_contact(ContactType type, b2Body* body)
		{
			if(is_contacting() && body != contact_body_ ||
				type == ContactType::End && !is_contacting()) // hack
			{
				// ignore multiple concurrent bodies
				return;
			}

			// TODO: this single body hack isn't going to cut it...
			switch(type)
			{
				case ContactType::Begin:
				++contact_count_;
				contact_body_ = body;
				break;

				case ContactType::End:
				--contact_count_;
				if(!is_contacting())
				{
					contact_body_ = nullptr;
				}
				break;
			}
		}
	
	}
}



