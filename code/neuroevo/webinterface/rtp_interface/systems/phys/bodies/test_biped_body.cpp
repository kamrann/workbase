// test_biped_body.cpp

#include "test_biped_body.h"
#include "../../../params/paramlist_par.h"

#include "Box2D/Box2D.h"


namespace rtp_phys {

	rtp_param_type* test_biped_body::spec::params()
	{
		rtp_named_param_list p = agent_body_spec::base_params();
		// Add more here
		return new rtp_staticparamlist_param_type(p);
	}

	test_biped_body::spec* test_biped_body::spec::create_instance(rtp_param param)
	{
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		return s;
	}

	agent_body* test_biped_body::spec::create_body(b2World* world)
	{
		return new test_biped_body(*this, world);
	}

	test_biped_body::spec::spec(): agent_body_spec(agent_body_spec::Type::TestBiped)
	{}

	test_biped_body::test_biped_body(spec const& spc, b2World* world)
	{
		m_torso_len = 1.5f;
		m_torso_width = 1.0f;
		m_pelvis_height = 1.0f;
		m_pelvis_width = 1.0f;
		m_upper_len = 2.0f;
		m_upper_width = 0.5f;
		m_lower_len = 2.0f;
		m_lower_width = 0.4f;
		m_foot_radius = 0.4f;
		float32 const initial_y = m_torso_len / 2 + m_pelvis_height / 2 + m_upper_len + m_lower_len + m_foot_radius + 0.1f;	// TODO: temp +0.1
		float32 const initial_hip_angle = 0;
		float32 const initial_knee_angle = 0;

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f, initial_y);
		bd.angle = 0.0f;
		torso = world->CreateBody(&bd);

		bd.position.Set(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2);
		pelvis = world->CreateBody(&bd);

		bd.position.Set(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2 - m_upper_len / 2);
		upper1 = world->CreateBody(&bd);
		upper2 = world->CreateBody(&bd);

		bd.position.Set(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2 - m_upper_len - m_lower_len / 2);
		lower1 = world->CreateBody(&bd);
		lower2 = world->CreateBody(&bd);

		b2PolygonShape torso_shape;
		torso_shape.SetAsBox(m_torso_width / 2, m_torso_len / 2);

		b2PolygonShape pelvis_shape;
		pelvis_shape.SetAsBox(m_pelvis_width / 2, m_pelvis_height / 2);

		b2PolygonShape upper_shape;
		upper_shape.SetAsBox(m_upper_width / 2, m_upper_len / 2);

		b2PolygonShape lower_shape;
		lower_shape.SetAsBox(m_lower_width / 2, m_lower_len / 2);

		b2CircleShape foot_shape;
		foot_shape.m_p.Set(0.0f, -m_lower_len / 2);
		foot_shape.m_radius = m_foot_radius;

		b2FixtureDef fd;
		fd.shape = &torso_shape;
		fd.density = 1.0f;
		fd.friction = 0.5f;
		fd.filter.groupIndex = -1;
		torso->CreateFixture(&fd);

		fd.shape = &pelvis_shape;
		fd.density = 1.5f;
		pelvis->CreateFixture(&fd);

		fd.shape = &upper_shape;
		fd.density = 1.0f;
		upper1->CreateFixture(&fd);
		upper2->CreateFixture(&fd);

		fd.shape = &lower_shape;
		lower1->CreateFixture(&fd);
		lower2->CreateFixture(&fd);

		fd.shape = &foot_shape;
		fd.friction = 0.7f;
		lower1->CreateFixture(&fd);
		lower2->CreateFixture(&fd);

		b2RevoluteJointDef jd;
		jd.Initialize(torso, pelvis, b2Vec2(0.0f, initial_y - m_torso_len / 2));
		jd.enableLimit = true;
		jd.lowerAngle = -b2_pi / 4;
		jd.upperAngle = b2_pi / 4;
		jd.enableMotor = false;// true;
		jd.motorSpeed = -1.0f;
		jd.maxMotorTorque = 100.0f;
		jd.collideConnected = false;
		pelvis_joint = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.lowerAngle = -b2_pi / 9;
		jd.upperAngle = b2_pi / 2;
		
		jd.Initialize(pelvis, upper1, b2Vec2(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2));
		hip1 = (b2RevoluteJoint*)world->CreateJoint(&jd);
		
		jd.Initialize(pelvis, upper2, b2Vec2(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2));
		hip2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.lowerAngle = -3 * b2_pi / 4;
		jd.upperAngle = 0;
		
		jd.Initialize(upper1, lower1, b2Vec2(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2 - m_upper_len));
		knee1 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(upper2, lower2, b2Vec2(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2 - m_upper_len));
		knee2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		m_bodies.push_back(torso);
		m_bodies.push_back(pelvis);
		m_bodies.push_back(upper1);
		m_bodies.push_back(upper2);
		m_bodies.push_back(lower1);
		m_bodies.push_back(lower2);
	}
/*
	void test_biped_body::draw(Wt::WPainter& painter)
	{
		Wt::WRectF rc_torso(
			-m_torso_width / 2,
			-m_torso_len / 2,
			m_torso_width,
			m_torso_len
			);
		painter.save();
		painter.translate(torso->GetPosition().x, torso->GetPosition().y);
		painter.rotate(torso->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_torso);
		painter.restore();

		Wt::WRectF rc_pelvis(
			-m_pelvis_width / 2,
			-m_pelvis_height / 2,
			m_pelvis_width,
			m_pelvis_height
			);
		painter.save();
		painter.translate(pelvis->GetPosition().x, pelvis->GetPosition().y);
		painter.rotate(pelvis->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_pelvis);
		painter.restore();

		Wt::WRectF rc_upper(
			-m_upper_width / 2,
			-m_upper_len / 2,
			m_upper_width,
			m_upper_len
			);
		painter.save();
		painter.translate(upper1->GetPosition().x, upper1->GetPosition().y);
		painter.rotate(upper1->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_upper);
		painter.restore();

		painter.save();
		painter.translate(upper2->GetPosition().x, upper2->GetPosition().y);
		painter.rotate(upper2->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_upper);
		painter.restore();

		Wt::WRectF rc_lower(
			-m_lower_width / 2,
			-m_lower_len / 2,
			m_lower_width,
			m_lower_len
			);
		Wt::WRectF rc_foot(
			-m_foot_radius,
			-m_lower_len / 2 - m_foot_radius,
			m_foot_radius * 2,
			m_foot_radius * 2
			);
		painter.save();
		painter.translate(lower1->GetPosition().x, lower1->GetPosition().y);
		painter.rotate(lower1->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_lower);
		painter.drawEllipse(rc_foot);
		painter.restore();

		painter.save();
		painter.translate(lower2->GetPosition().x, lower2->GetPosition().y);
		painter.rotate(lower2->GetAngle() * 360.0 / (2 * b2_pi));
		painter.drawRect(rc_lower);
		painter.drawEllipse(rc_foot);
		painter.restore();
	}
*/
}



