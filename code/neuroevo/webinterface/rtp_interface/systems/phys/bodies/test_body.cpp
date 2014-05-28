// test_body.cpp

#include "test_body.h"
#include "../../../params/paramlist_par.h"

#include "Box2D/Box2D.h"


namespace rtp_phys {

	rtp_param_type* test_body::spec::params()
	{
		rtp_named_param_list p = agent_body_spec::base_params();
		// Add more here
		return new rtp_staticparamlist_param_type(p);
	}

	test_body::spec* test_body::spec::create_instance(rtp_param param)
	{
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		return s;
	}

	agent_body* test_body::spec::create_body(b2World* world)
	{
		return new test_body(*this, world);
	}

	test_body::spec::spec(): agent_body_spec(agent_body_spec::Type::TestCreature)
	{}

	test_body::test_body(spec const& spc, b2World* world)
	{
		float32 const initial_y = 2.5f;
		m_rear_len = 4.0f;
		m_fore_len = 2.0f;
		m_width = 0.8f;
		float32 const initial_rear_angle = b2_pi / 6;
		float32 const initial_fore_angle = -b2_pi / 6;

		b2BodyDef bd1;
		bd1.type = b2_dynamicBody;
		bd1.position.Set(
			-(m_rear_len / 2) * std::cos(initial_rear_angle),
			initial_y - (m_rear_len / 2) * std::sin(initial_rear_angle));
		bd1.angle = initial_rear_angle;
		rear = world->CreateBody(&bd1);

		b2BodyDef bd2;
		bd2.type = b2_dynamicBody;
		bd2.position.Set(
			(m_fore_len / 2) * std::cos(initial_fore_angle),
			initial_y + (m_fore_len / 2) * std::sin(initial_fore_angle));
		bd2.angle = initial_fore_angle;
		fore = world->CreateBody(&bd2);

		b2PolygonShape shape1;
		shape1.SetAsBox(m_rear_len / 2, m_width / 2);

		b2PolygonShape shape2;
		shape2.SetAsBox(m_fore_len / 2, m_width / 2);

		b2FixtureDef fd;
		fd.shape = &shape1;
		fd.density = 1.0f;
		fd.friction = 0.4f;
		rear->CreateFixture(&fd);

		fd.shape = &shape2;
		fd.density = 1.5f;
		fd.friction = 0.7f;
		fore->CreateFixture(&fd);

		b2RevoluteJointDef jd;
		jd.Initialize(rear, fore, b2Vec2(0.0f, initial_y));
		jd.enableLimit = true;
		jd.lowerAngle = -b2_pi / 6;
		jd.upperAngle = b2_pi / 9;
		jd.enableMotor = false;// true;
		jd.motorSpeed = -1.0f;
		jd.maxMotorTorque = 100.0f;
		jd.collideConnected = false;

		joint = (b2RevoluteJoint*)world->CreateJoint(&jd);

		m_bodies.push_back(rear);
		m_bodies.push_back(fore);
	}

/*
	void test_body::draw(Wt::WPainter& painter)
	{
		painter.save();
		painter.translate(rear->GetPosition().x, rear->GetPosition().y);
		painter.rotate(rear->GetAngle() * 360.0 / (2 * b2_pi));

		Wt::WRectF rc_rear(
			-m_rear_len / 2,
			-m_width / 2,
			m_rear_len,
			m_width
			);
		painter.drawRect(rc_rear);
		painter.restore();

		painter.save();
		painter.translate(fore->GetPosition().x, fore->GetPosition().y);
		painter.rotate(fore->GetAngle() * 360.0 / (2 * b2_pi));

		Wt::WRectF rc_fore(
			-m_fore_len / 2,
			-m_width / 2,
			m_fore_len,
			m_width
			);
		painter.drawRect(rc_fore);
		painter.restore();
	}
*/
}



