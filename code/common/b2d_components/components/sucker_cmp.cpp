// sucker_cmp.cpp

#include "sucker_cmp.h"


namespace b2dc {

	std::shared_ptr< sucker_cmp > sucker_cmp::create(
		b2Body* body
		, b2Vec2 local_point
		, double range
		//			, double max_force
		, collision_filter sensor_filter
		)
	{
		auto sck = sucker::create(body, local_point);

		b2CircleShape sensor_shape;
		sensor_shape.m_p = local_point;
		sensor_shape.m_radius = range;
		auto sen = basic_contact_sensor::create(&sensor_shape, body, sensor_filter);

		return std::make_shared< sucker_cmp >(sck, sen);
	}
	
	sucker_cmp::sucker_cmp(std::shared_ptr< sucker > sck, std::shared_ptr< basic_contact_sensor > sen):
		sucker_{ sck }
		, sensor_{ sen }
	{

	}

}





