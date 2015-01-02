// magnet.cpp

#include "magnet.h"


namespace b2dc {

	std::shared_ptr< magnet > magnet::create(
		b2Body* body
		, b2Vec2 local_point
		, double strength
		)
	{
		return std::make_shared< magnet >(body, local_point, strength);
	}

	magnet::magnet(b2Body* body, b2Vec2 local_point, double strength):
		body_{ body }
		, local_point_{ local_point }
		, strength_{ strength }
	{
		// Acceleration of a metallic object in the field will be strength/distance^2
		auto const dropoff_accel = 0.1;	// todo: specifiable
		auto dropoff_radius = std::sqrt(strength / dropoff_accel);

		b2CircleShape shape;
		shape.m_p = local_point;
		shape.m_radius = dropoff_radius;

		b2FixtureDef fd;
		fd.isSensor = true;
		fd.shape = &shape;
		//fd.filter = metallic objects??

		sensor_ = body_->CreateFixture(&fd);
	}

	void magnet::switch_on()
	{
		active_ = true;
	}

	void magnet::switch_off()
	{
		active_ = false;
	}

	void magnet::update()
	{
		if(!is_activated())
		{
			return;
		}

		auto mag_pos = body_->GetWorldPoint(local_point_);

		auto ce = body_->GetContactList();
		//			std::set< b2Body* > processed;
		while(ce)
		{
			auto contact = ce->contact;
			if(contact->IsTouching())
			{
				auto fixA = contact->GetFixtureA();
				auto fixB = contact->GetFixtureB();

				//					b2Body* obj = nullptr;
				b2Fixture* fix = nullptr;
				if(fixA == sensor_)
				{
					//						obj = fixB->GetBody();
					fix = fixB;
				}
				else if(fixB == sensor_)
				{
					//						obj = fixA->GetBody();
					fix = fixA;
				}

				//					if(obj != nullptr && processed.find(obj) == processed.end())
				if(fix)
				{
					b2MassData md;
					fix->GetMassData(&md);
					auto metallic_mass = md.mass; // TODO: composition
					auto obj_pos = fix->GetBody()->GetWorldPoint(md.center);
					auto vec = mag_pos - obj_pos;
					auto sqr_dist = vec.LengthSquared();
					auto magnitude = strength_ * metallic_mass / sqr_dist;

					vec *= magnitude / std::sqrt(sqr_dist);
					fix->GetBody()->ApplyForce(vec, obj_pos, true);
					body_->ApplyForce(-vec, mag_pos, true);

					//						processed.insert(obj);
				}
			}

			ce = ce->next;
		}
	}

}






