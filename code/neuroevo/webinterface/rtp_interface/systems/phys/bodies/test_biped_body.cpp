// test_biped_body.cpp

#include "test_biped_body.h"
#include "../rtp_phys_system.h"
#include "../rtp_phys_entity_data.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include "Box2D/Box2D.h"


namespace rtp {

	const double test_biped_body::spec::DefaultMaxKneeTorque = 75;
	const double test_biped_body::spec::DefaultMaxHipTorque = 75;
	const double test_biped_body::spec::DefaultMaxElbowTorque = 40;
	const double test_biped_body::spec::DefaultMaxShoulderTorque = 40;
	const double test_biped_body::spec::DefaultMaxPelvicTorque = 125;
	const double test_biped_body::spec::DefaultMaxNeckTorque = 20;

	namespace sb = prm::schema;

	prm::schema::schema_node test_biped_body::spec::get_schema(prm::param_accessor param_vals)
	{
		auto schema = sb::list("biped");
		sb::append(schema, sb::real("max_knee_torque", DefaultMaxKneeTorque));
		sb::label(sb::last(schema), "Max Knee Torque");
		sb::append(schema, sb::real("max_hip_torque", DefaultMaxHipTorque));
		sb::label(sb::last(schema), "Max Hip Torque");
		sb::append(schema, sb::real("max_elbow_torque", DefaultMaxElbowTorque));
		sb::label(sb::last(schema), "Max Elbow Torque");
		sb::append(schema, sb::real("max_shoulder_torque", DefaultMaxShoulderTorque));
		sb::label(sb::last(schema), "Max Shoulder Torque");
		sb::append(schema, sb::real("max_pelvic_torque", DefaultMaxPelvicTorque));
		sb::label(sb::last(schema), "Max Pelvic Torque");
		sb::append(schema, sb::real("max_neck_torque", DefaultMaxNeckTorque));
		sb::label(sb::last(schema), "Max Neck Torque");
		return schema;
	}

	std::string test_biped_body::spec::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		float const MaxPelvicTorque = 125.0f;
		float const MaxHipTorque = 75.0f;
		float const MaxKneeTorque = 75.0f;
		float const MaxShoulderTorque = 40.0f;
		float const MaxElbowTorque = 40.0f;
		float const MaxNeckTorque = 20.0f;

		auto relative = std::string{ "biped" };
		auto path = prefix + relative;

		path += std::string{ "max_torques" };

		(*provider)[path + std::string("max_knee_torque")] = [=](prm::param_accessor)
		{
			auto s = sb::real("max_knee_torque", MaxKneeTorque);
			sb::label(s, "Knee");
			return s;
		};

		(*provider)[path + std::string("max_hip_torque")] = [=](prm::param_accessor)
		{
			auto s = sb::real("max_hip_torque", MaxHipTorque);
			sb::label(s, "Hip");
			return s;
		};

		(*provider)[path + std::string("max_elbow_torque")] = [=](prm::param_accessor)
		{
			auto s = sb::real("max_elbow_torque", MaxElbowTorque);
			sb::label(s, "Elbow");
			return s;
		};

		(*provider)[path + std::string("max_shoulder_torque")] = [=](prm::param_accessor)
		{
			auto s = sb::real("max_shoulder_torque", MaxShoulderTorque);
			sb::label(s, "Shoulder");
			return s;
		};

		(*provider)[path + std::string("max_pelvic_torque")] = [=](prm::param_accessor)
		{
			auto s = sb::real("max_pelvic_torque", MaxPelvicTorque);
			sb::label(s, "Pelvic");
			return s;
		};

		(*provider)[path + std::string("max_neck_torque")] = [=](prm::param_accessor)
		{
			auto s = sb::real("max_neck_torque", MaxNeckTorque);
			sb::label(s, "Neck");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("max_torques");
			sb::append(s, provider->at(path + std::string("max_knee_torque"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_hip_torque"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_elbow_torque"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_shoulder_torque"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_pelvic_torque"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_neck_torque"))(param_vals));
			sb::label(s, "Max Torques");
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("max_torques"))(param_vals));
			return s;
		};

		return relative;
	}

	test_biped_body::spec* test_biped_body::spec::create_instance(prm::param_accessor param)
	{
		param.push_relative_path(prm::qualified_path("biped"));
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		s->max_knee_torque = param["max_knee_torque"].as< double >();
		s->max_hip_torque = param["max_hip_torque"].as< double >();
		s->max_elbow_torque = param["max_elbow_torque"].as< double >();
		s->max_shoulder_torque = param["max_shoulder_torque"].as< double >();
		s->max_pelvic_torque = param["max_pelvic_torque"].as< double >();
		s->max_neck_torque = param["max_neck_torque"].as< double >();
		param.pop_relative_path();
		return s;
	}

	test_biped_body::spec* test_biped_body::spec::create_instance(prm::param& param)
	{
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		prm::extract_to(param["max_knee_torque"], s->max_knee_torque);
		prm::extract_to(param["max_hip_torque"], s->max_hip_torque);
		prm::extract_to(param["max_elbow_torque"], s->max_elbow_torque);
		prm::extract_to(param["max_shoulder_torque"], s->max_shoulder_torque);
		prm::extract_to(param["max_pelvic_torque"], s->max_pelvic_torque);
		prm::extract_to(param["max_neck_torque"], s->max_neck_torque);
		return s;
	}

	agent_sensor_name_list test_biped_body::spec::sensor_inputs()
	{
		auto inputs = composite_rigid_body::sensor_inputs();
		inputs.insert(
			end(inputs),
			{
			std::string("Left Knee Angle"),
			std::string("Left Knee Speed"),
			std::string("Right Knee Angle"),
			std::string("Right Knee Speed"),
			std::string("Left Hip Angle"),
			std::string("Left Hip Speed"),
			std::string("Right Hip Angle"),
			std::string("Right Hip Speed"),
			std::string("Abdomen Angle"),
			std::string("Abdomen Speed"),
			std::string("Left Elbow Angle"),
			std::string("Left Elbow Speed"),
			std::string("Right Elbow Angle"),
			std::string("Right Elbow Speed"),
			std::string("Left Shoulder Angle"),
			std::string("Left Shoulder Speed"),
			std::string("Right Shoulder Angle"),
			std::string("Right Shoulder Speed"),
			std::string("Neck Angle"),
			std::string("Neck Speed"),
			std::string("Balance"),		// Head orientation, 0 == level
			std::string("Non-Foot Contact Count"),
			std::string("Head Height"),
			});
		return inputs;
	}

	size_t test_biped_body::spec::num_effectors()
	{
		return 10;
	}

	agent_body* test_biped_body::spec::create_body(b2World* world)
	{
		return nullptr;// new test_biped_body(*this, world);
	}

	test_biped_body::spec::spec(): agent_body_spec(agent_body_spec::Type::TestBiped)
	{}

	test_biped_body::test_biped_body(phys_agent_specification const& spec, phys_system* system) : composite_rigid_body(spec, system)
	{
		auto world = system->get_world();

		m_torso_len = 1.5f;
		m_torso_width = 1.0f;
		m_pelvis_height = 1.0f;
		m_pelvis_width = 1.0f;
		m_upper_len = 2.0f;
		m_upper_width = 0.5f;
		m_lower_len = 2.0f;
		m_lower_width = 0.4f;
		m_foot_radius = 0.4f;
		m_upper_arm_len = 1.5f;
		m_upper_arm_width = 0.3f;
		m_forearm_len = 2.0f;
		m_forearm_width = 0.3f;
		m_head_radius = 0.4;
		float32 const initial_y = m_torso_len / 2 + m_pelvis_height / 2 + m_upper_len + m_lower_len + m_foot_radius + 0.1f;	// TODO: temp +0.1
		float32 const initial_hip_angle = 0;
		float32 const initial_knee_angle = 0;
		float32 const initial_shoulder_angle = 0;
		float32 const initial_elbow_angle = 0;
		float32 const initial_neck_angle = 0;

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

		bd.position.Set(0.0f, initial_y + m_torso_len / 2 - m_upper_arm_len / 2);
		upper_arm1 = world->CreateBody(&bd);
		upper_arm2 = world->CreateBody(&bd);

		bd.position.Set(0.0f, initial_y + m_torso_len / 2 - m_upper_arm_len - m_forearm_len / 2);
		forearm1 = world->CreateBody(&bd);
		forearm2 = world->CreateBody(&bd);

		bd.position.Set(0.0f, initial_y + m_torso_len / 2 + m_head_radius);
		head = world->CreateBody(&bd);

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

		b2PolygonShape upper_arm_shape;
		upper_arm_shape.SetAsBox(m_upper_arm_width / 2, m_upper_arm_len / 2);

		b2PolygonShape forearm_shape;
		forearm_shape.SetAsBox(m_forearm_width / 2, m_forearm_len / 2);

		b2CircleShape head_shape;
		head_shape.m_p.Set(0.0f, 0.0f);
		head_shape.m_radius = m_head_radius;

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

		auto foot1 = lower1->CreateFixture(&fd);
		auto foot2 = lower2->CreateFixture(&fd);

		// Set user data to be able to distinguish foot fixtures
		set_fixture_data(foot1, entity_fix_data{ entity_fix_data::Type::Other, entity_fix_data::val_t{} });
		set_fixture_data(foot2, entity_fix_data{ entity_fix_data::Type::Other, entity_fix_data::val_t{} });

		fd.friction = 0.5f;
		fd.shape = &upper_arm_shape;
		upper_arm1->CreateFixture(&fd);
		upper_arm2->CreateFixture(&fd);

		fd.shape = &forearm_shape;
		forearm1->CreateFixture(&fd);
		forearm2->CreateFixture(&fd);

		fd.shape = &head_shape;
		head->CreateFixture(&fd);

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

		// Hip joints
		jd.lowerAngle = -b2_pi / 9;
		jd.upperAngle = b2_pi / 2;
		
		jd.Initialize(pelvis, upper1, b2Vec2(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2));
		hip1 = (b2RevoluteJoint*)world->CreateJoint(&jd);
		
		jd.Initialize(pelvis, upper2, b2Vec2(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2));
		hip2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		// Knee joints
		jd.lowerAngle = -3 * b2_pi / 4;
		jd.upperAngle = 0;
		
		jd.Initialize(upper1, lower1, b2Vec2(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2 - m_upper_len));
		knee1 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(upper2, lower2, b2Vec2(0.0f, initial_y - m_torso_len / 2 - m_pelvis_height / 2 - m_upper_len));
		knee2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		// Shoulder joints
		jd.lowerAngle = -b2_pi / 4;
		jd.upperAngle = 3 * b2_pi / 4;

		jd.Initialize(torso, upper_arm1, b2Vec2(0.0f, initial_y + m_torso_len / 2));
		shoulder1 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(torso, upper_arm2, b2Vec2(0.0f, initial_y + m_torso_len / 2));
		shoulder2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		// Elbow joints
		jd.lowerAngle = 0;
		jd.upperAngle = b2_pi;

		jd.Initialize(upper_arm1, forearm1, b2Vec2(0.0f, initial_y + m_torso_len / 2 - m_upper_arm_len));
		elbow1 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(upper_arm2, forearm2, b2Vec2(0.0f, initial_y + m_torso_len / 2 - m_upper_arm_len));
		elbow2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		// Neck joint
		jd.lowerAngle = -b2_pi / 4;
		jd.upperAngle = b2_pi / 4;

		jd.Initialize(head, torso, b2Vec2(0.0f, initial_y + m_torso_len / 2));
		neck_joint = (b2RevoluteJoint*)world->CreateJoint(&jd);

		add_component_body(torso, entity_data::val_t{});
		add_component_body(pelvis, entity_data::val_t{});
		add_component_body(upper1, entity_data::val_t{});
		add_component_body(upper2, entity_data::val_t{});
		add_component_body(lower1, entity_data::val_t{});
		add_component_body(lower2, entity_data::val_t{});
		add_component_body(upper_arm1, entity_data::val_t{});
		add_component_body(upper_arm2, entity_data::val_t{});
		add_component_body(forearm1, entity_data::val_t{});
		add_component_body(forearm2, entity_data::val_t{});
		add_component_body(head, entity_data::val_t{});

		m_max_knee_torque = spec.spec_acc["max_knee_torque"].as< double >();
		m_max_hip_torque = spec.spec_acc["max_hip_torque"].as< double >();
		m_max_elbow_torque = spec.spec_acc["max_elbow_torque"].as< double >();
		m_max_shoulder_torque = spec.spec_acc["max_shoulder_torque"].as< double >();
		m_max_pelvic_torque = spec.spec_acc["max_pelvic_torque"].as< double >();
		m_max_neck_torque = spec.spec_acc["max_neck_torque"].as< double >();

		m_non_foot_contact_count = 0;
	}

	double test_biped_body::get_sensor_value(agent_sensor_id const& sensor) const
	{
		switch(sensor)
		{
			case spec::Sensors::LeftKneeAngle:
			return knee1->GetJointAngle();
			case spec::Sensors::LeftKneeSpeed:
			return knee1->GetJointSpeed();
			case spec::Sensors::RightKneeAngle:
			return knee2->GetJointAngle();
			case spec::Sensors::RightKneeSpeed:
			return knee2->GetJointSpeed();
			case spec::Sensors::LeftHipAngle:
			return hip1->GetJointAngle();
			case spec::Sensors::LeftHipSpeed:
			return hip1->GetJointSpeed();
			case spec::Sensors::RightHipAngle:
			return hip2->GetJointAngle();
			case spec::Sensors::RightHipSpeed:
			return hip2->GetJointSpeed();
			case spec::Sensors::AbdomenAngle:
			return pelvis_joint->GetJointAngle();
			case spec::Sensors::AbdomenSpeed:
			return pelvis_joint->GetJointSpeed();
			case spec::Sensors::LeftElbowAngle:
			return elbow1->GetJointAngle();
			case spec::Sensors::LeftElbowSpeed:
			return elbow1->GetJointSpeed();
			case spec::Sensors::RightElbowAngle:
			return elbow2->GetJointAngle();
			case spec::Sensors::RightElbowSpeed:
			return elbow2->GetJointSpeed();
			case spec::Sensors::LeftShoulderAngle:
			return shoulder1->GetJointAngle();
			case spec::Sensors::LeftShoulderSpeed:
			return shoulder1->GetJointSpeed();
			case spec::Sensors::RightShoulderAngle:
			return shoulder2->GetJointAngle();
			case spec::Sensors::RightShoulderSpeed:
			return shoulder2->GetJointSpeed();
			case spec::Sensors::NeckAngle:
			return neck_joint->GetJointAngle();
			case spec::Sensors::NeckSpeed:
			return neck_joint->GetJointSpeed();
			case spec::Sensors::Balance:
			return head->GetAngle();
			case spec::Sensors::NonFootContactCount:
			return (double)m_non_foot_contact_count;
			case spec::Sensors::HeadHeight:
			return head->GetPosition().y;
			default:
			return composite_rigid_body::get_sensor_value(sensor);
		}
	}

	void test_biped_body::activate_effectors(std::vector< double > const& activations)
	{
		agent_body::activate_effectors(activations);

		torso->ApplyTorque(-activations[0] * m_max_pelvic_torque, true);
		pelvis->ApplyTorque(activations[0] * m_max_pelvic_torque, true);
		pelvis->ApplyTorque(-activations[1] * m_max_hip_torque, true);
		upper1->ApplyTorque(activations[1] * m_max_hip_torque, true);
		pelvis->ApplyTorque(-activations[2] * m_max_hip_torque, true);
		upper2->ApplyTorque(activations[2] * m_max_hip_torque, true);
		upper1->ApplyTorque(-activations[3] * m_max_knee_torque, true);
		lower1->ApplyTorque(activations[3] * m_max_knee_torque, true);
		upper2->ApplyTorque(-activations[4] * m_max_knee_torque, true);
		lower2->ApplyTorque(activations[4] * m_max_knee_torque, true);
		torso->ApplyTorque(-activations[5] * m_max_shoulder_torque, true);
		upper_arm1->ApplyTorque(activations[5] * m_max_shoulder_torque, true);
		torso->ApplyTorque(-activations[6] * m_max_shoulder_torque, true);
		upper_arm2->ApplyTorque(activations[6] * m_max_shoulder_torque, true);
		upper_arm1->ApplyTorque(-activations[7] * m_max_elbow_torque, true);
		forearm1->ApplyTorque(activations[7] * m_max_elbow_torque, true);
		upper_arm2->ApplyTorque(-activations[8] * m_max_elbow_torque, true);
		forearm2->ApplyTorque(activations[8] * m_max_elbow_torque, true);
		head->ApplyTorque(-activations[9] * m_max_neck_torque, true);
		torso->ApplyTorque(activations[9] * m_max_neck_torque, true);
	}

	void test_biped_body::on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type)
		//b2Contact* contact)
	{
		// We know fixA is part of us, and also that we are not colliding limbs with each other, so fixB is external (assuming ground)
		auto fd = get_fixture_data(fixA);
		if(fd == nullptr && type == ContactType::Begin)	// TODO:
		{
			// Non-foot contact
			++m_non_foot_contact_count;
		}
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



