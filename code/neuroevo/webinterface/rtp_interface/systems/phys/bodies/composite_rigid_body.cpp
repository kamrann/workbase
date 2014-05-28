// composite_rigid_body.cpp

#include "composite_rigid_body.h"

#include "../b2d_util.h"

//
#include <Wt/WPainter>
//

#include <Box2D/Box2D.h>


namespace rtp_phys {

	void composite_rigid_body::translate(b2Vec2 const& vec)
	{
		for(b2Body* b : m_bodies)
		{
			b->SetTransform(b->GetPosition() + vec, b->GetTransform().q.GetAngle());
		}
	}

	void composite_rigid_body::rotate(float angle)
	{
		// Rotate about center of mass
		b2Vec2 com = get_com_position();
		for(b2Body* b : m_bodies)
		{
			// TODO: This definitely correct method???
			b2Vec2 pos = b->GetPosition();
			pos -= com;
			pos = b2Vec2((pos.x * std::cos(angle)) + (pos.y * -std::sin(angle)),
				(pos.x * std::sin(angle)) + (pos.y * std::cos(angle)));
			pos += com;
			b->SetTransform(pos, b->GetTransform().q.GetAngle() + angle);
		}
	}

	void composite_rigid_body::set_linear_velocity(b2Vec2 const& linvel)
	{
		for(b2Body* b : m_bodies)
		{
			b->SetLinearVelocity(linvel);
		}
	}

	void composite_rigid_body::set_angular_velocity(float angvel)
	{
		b2Vec2 com = get_com_position();
		b2Vec2 v_com = get_com_linear_velocity();
		for(b2Body* b : m_bodies)
		{
			b->SetAngularVelocity(angvel);
			b2Vec2 r = b->GetPosition() - com;
			b->SetLinearVelocity(v_com + angvel * b2Vec2(-r.y, r.x));
		}
	}

	b2Vec2 composite_rigid_body::get_position() const
	{
		return get_com_position();
	}

	b2Vec2 composite_rigid_body::get_linear_velocity() const
	{
		return get_com_linear_velocity();
	}

	float composite_rigid_body::get_kinetic_energy() const
	{
		float ke = 0.0f;
		for(b2Body const* b : m_bodies)
		{
			ke += ::get_kinetic_energy(b);
		}
		return ke;
	}

	b2Vec2 composite_rigid_body::get_com_position() const
	{
		b2Vec2 com(0.0f, 0.0f);
		float sum_mass = 0.0f;
		for(b2Body const* b : m_bodies)
		{
			float mass = b->GetMass();
			com += mass * b->GetPosition();
			sum_mass += mass;
		}
		return (1.0f / sum_mass) * com;
	}
	
	b2Vec2 composite_rigid_body::get_com_linear_velocity() const
	{
		b2Vec2 com_vel(0.0f, 0.0f);
		float sum_mass = 0.0f;
		for(b2Body const* b : m_bodies)
		{
			float mass = b->GetMass();
			com_vel += mass * b->GetLinearVelocity();
			sum_mass += mass;
		}
		return (1.0f / sum_mass) * com_vel;
	}
	
	void composite_rigid_body::draw(Wt::WPainter& painter) const
	{
		for(b2Body const* b : m_bodies)
		{
			::draw_body(b, painter);
		}
	}
}


