// test_quadruped_body.cpp

#include "test_quadruped_body.h"
#include "../../../params/paramlist_par.h"

#include "Box2D/Box2D.h"


namespace rtp_phys {

	rtp_param_type* test_quadruped_body::spec::params()
	{
		rtp_named_param_list p = agent_body_spec::base_params();
		// Add more here
		return new rtp_staticparamlist_param_type(p);
	}

	test_quadruped_body::spec* test_quadruped_body::spec::create_instance(rtp_param param)
	{
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		return s;
	}

	agent_body* test_quadruped_body::spec::create_body(b2World* world)
	{
		return new test_quadruped_body(*this, world);
	}

	test_quadruped_body::spec::spec(): agent_body_spec(agent_body_spec::Type::TestQuadruped)
	{}

	test_quadruped_body::test_quadruped_body(spec const& spc, b2World* world)
	{
		m_torso_len = 4.0f;
		m_torso_width = 1.0f;
		m_upper_len = 1.0f;
		m_upper_width = 0.5f;
		m_lower_len = 1.0f;
		m_lower_width = 0.4f;
		float32 const initial_y = m_torso_width / 2 + m_upper_len + m_lower_len + 0.1f;	// TODO: temp +0.1
		float32 const initial_hip_angle = 0;
		float32 const initial_knee_angle = 0;

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f, initial_y);
		bd.angle = 0.0f;
		torso = world->CreateBody(&bd);

		bd.position.Set(-m_torso_len / 2, initial_y - m_torso_width / 2 - m_upper_len / 2);
		rear_upper1 = world->CreateBody(&bd);
		rear_upper2 = world->CreateBody(&bd);
		bd.position.Set(m_torso_len / 2, initial_y - m_torso_width / 2 - m_upper_len / 2);
		fore_upper1 = world->CreateBody(&bd);
		fore_upper2 = world->CreateBody(&bd);

		bd.position.Set(-m_torso_len / 2, initial_y - m_torso_width / 2 - m_upper_len - m_lower_len / 2);
		rear_lower1 = world->CreateBody(&bd);
		rear_lower2 = world->CreateBody(&bd);
		bd.position.Set(m_torso_len / 2, initial_y - m_torso_width / 2 - m_upper_len - m_lower_len / 2);
		fore_lower1 = world->CreateBody(&bd);
		fore_lower2 = world->CreateBody(&bd);

		b2PolygonShape torso_shape;
		torso_shape.SetAsBox(m_torso_len / 2, m_torso_width / 2);

		b2PolygonShape upper_shape;
		upper_shape.SetAsBox(m_upper_width / 2, m_upper_len / 2);

		b2PolygonShape lower_shape;
		lower_shape.SetAsBox(m_lower_width / 2, m_lower_len / 2);

		b2FixtureDef fd;
		fd.shape = &torso_shape;
		fd.density = 1.0f;
		fd.friction = 0.5f;
		fd.filter.groupIndex = -1;
		torso->CreateFixture(&fd);

		fd.shape = &upper_shape;
		rear_upper1->CreateFixture(&fd);
		rear_upper2->CreateFixture(&fd);
		fore_upper1->CreateFixture(&fd);
		fore_upper2->CreateFixture(&fd);

		fd.shape = &lower_shape;
		fd.friction = 0.8f;
		rear_lower1->CreateFixture(&fd);
		rear_lower2->CreateFixture(&fd);
		fore_lower1->CreateFixture(&fd);
		fore_lower2->CreateFixture(&fd);

		b2RevoluteJointDef jd;
		jd.Initialize(torso, rear_upper1, b2Vec2(-m_torso_len / 2, initial_y - m_torso_width / 2));
		jd.enableLimit = true;
		jd.lowerAngle = -b2_pi / 9;
		jd.upperAngle = b2_pi / 2;
		jd.enableMotor = false;// true;
		jd.motorSpeed = -1.0f;
		jd.maxMotorTorque = 100.0f;
		jd.collideConnected = false;
		rear_hip1 = (b2RevoluteJoint*)world->CreateJoint(&jd);
		
		jd.Initialize(torso, rear_upper2, b2Vec2(-m_torso_len / 2, initial_y - m_torso_width / 2));
		rear_hip2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(torso, fore_upper1, b2Vec2(m_torso_len / 2, initial_y - m_torso_width / 2));
		fore_hip1 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(torso, fore_upper2, b2Vec2(m_torso_len / 2, initial_y - m_torso_width / 2));
		fore_hip2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(rear_upper1, rear_lower1, b2Vec2(-m_torso_len / 2, initial_y - m_torso_width / 2 - m_upper_len));
		jd.lowerAngle = -3 * b2_pi / 4;
		jd.upperAngle = 0;
		rear_knee1 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(rear_upper2, rear_lower2, b2Vec2(-m_torso_len / 2, initial_y - m_torso_width / 2 - m_upper_len));
		rear_knee2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(fore_upper1, fore_lower1, b2Vec2(m_torso_len / 2, initial_y - m_torso_width / 2 - m_upper_len));
		fore_knee1 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(fore_upper2, fore_lower2, b2Vec2(m_torso_len / 2, initial_y - m_torso_width / 2 - m_upper_len));
		fore_knee2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		m_bodies.push_back(torso);
		m_bodies.push_back(rear_upper1);
		m_bodies.push_back(rear_upper2);
		m_bodies.push_back(rear_lower1);
		m_bodies.push_back(rear_lower2);
		m_bodies.push_back(fore_upper1);
		m_bodies.push_back(fore_upper2);
		m_bodies.push_back(fore_lower1);
		m_bodies.push_back(fore_lower2);
	}
/*
	void test_quadruped_body::draw(Wt::WPainter& painter)
	{
		Wt::WRectF rc_torso(
			-m_torso_len / 2,
			-m_torso_width / 2,
			m_torso_len,
			m_torso_width
			);
		painter.save();
		painter.translate(torso->GetPosition().x, torso->GetPosition().y);
		painter.rotate(torso->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_torso);
		painter.restore();

		Wt::WRectF rc_upper(
			-m_upper_width / 2,
			-m_upper_len / 2,
			m_upper_width,
			m_upper_len
			);
		painter.save();
		painter.translate(rear_upper1->GetPosition().x, rear_upper1->GetPosition().y);
		painter.rotate(rear_upper1->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_upper);
		painter.restore();

		painter.save();
		painter.translate(rear_upper2->GetPosition().x, rear_upper2->GetPosition().y);
		painter.rotate(rear_upper2->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_upper);
		painter.restore();

		painter.save();
		painter.translate(fore_upper1->GetPosition().x, fore_upper1->GetPosition().y);
		painter.rotate(fore_upper1->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_upper);
		painter.restore();

		painter.save();
		painter.translate(fore_upper2->GetPosition().x, fore_upper2->GetPosition().y);
		painter.rotate(fore_upper2->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_upper);
		painter.restore();

		Wt::WRectF rc_lower(
			-m_lower_width / 2,
			-m_lower_len / 2,
			m_lower_width,
			m_lower_len
			);
		painter.save();
		painter.translate(rear_lower1->GetPosition().x, rear_lower1->GetPosition().y);
		painter.rotate(rear_lower1->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_lower);
		painter.restore();

		painter.save();
		painter.translate(rear_lower2->GetPosition().x, rear_lower2->GetPosition().y);
		painter.rotate(rear_lower2->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_lower);
		painter.restore();

		painter.save();
		painter.translate(fore_lower1->GetPosition().x, fore_lower1->GetPosition().y);
		painter.rotate(fore_lower1->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_lower);
		painter.restore();

		painter.save();
		painter.translate(fore_lower2->GetPosition().x, fore_lower2->GetPosition().y);
		painter.rotate(fore_lower2->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_lower);
		painter.restore();
	}
*/
}



