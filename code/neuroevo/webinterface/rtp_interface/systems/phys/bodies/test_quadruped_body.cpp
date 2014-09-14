// test_quadruped_body.cpp

#include "test_quadruped_body.h"
#include "../rtp_phys_entity_data.h"
#include "../rtp_phys_system.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include "Box2D/Box2D.h"


namespace rtp {

	namespace sb = prm::schema;

	prm::schema::schema_node test_quadruped_body::spec::get_schema(prm::param_accessor param_vals)
	{
		auto schema = sb::list("quadruped");
		//sb::append(schema, sb::integer("Placeholder", 0));
		return schema;
	}

	std::string test_quadruped_body::spec::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "quadruped" };
		auto path = prefix + relative;

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			return s;
		};

		return relative;
	}

	test_quadruped_body::spec* test_quadruped_body::spec::create_instance(prm::param_accessor param)
	{
		param.push_relative_path(prm::qualified_path("quadruped"));
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		param.pop_relative_path();
		return s;
	}

	test_quadruped_body::spec* test_quadruped_body::spec::create_instance(prm::param& param)
	{
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		return s;
	}

	agent_sensor_name_list test_quadruped_body::spec::sensor_inputs()
	{
		auto inputs = composite_rigid_body::sensor_inputs();
		inputs.insert(
			end(inputs),
			{
			std::string("Rear Left Knee Angle"),
			std::string("Rear Left Knee Speed"),
			std::string("Rear Right Knee Angle"),
			std::string("Rear Right Knee Speed"),
			std::string("Rear Left Hip Angle"),
			std::string("Rear Left Hip Speed"),
			std::string("Rear Right Hip Angle"),
			std::string("Rear Right Hip Speed"),
			std::string("Fore Left Knee Angle"),
			std::string("Fore Left Knee Speed"),
			std::string("Fore Right Knee Angle"),
			std::string("Fore Right Knee Speed"),
			std::string("Fore Left Hip Angle"),
			std::string("Fore Left Hip Speed"),
			std::string("Fore Right Hip Angle"),
			std::string("Fore Right Hip Speed"),
			});
		return inputs;
	}

	size_t test_quadruped_body::spec::num_effectors()
	{
		return 8;
	}


	agent_body* test_quadruped_body::spec::create_body(b2World* world)
	{
		return nullptr;// new test_quadruped_body(*this, world);
	}

	test_quadruped_body::spec::spec(): agent_body_spec(agent_body_spec::Type::TestQuadruped)
	{}

	test_quadruped_body::test_quadruped_body(phys_agent_specification const& spec, phys_system* system) : composite_rigid_body(spec, system)
	{
		auto world = system->get_world();

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

		add_component_body(torso, entity_data::val_t{});
		add_component_body(rear_upper1, entity_data::val_t{});
		add_component_body(rear_upper2, entity_data::val_t{});
		add_component_body(rear_lower1, entity_data::val_t{});
		add_component_body(rear_lower2, entity_data::val_t{});
		add_component_body(fore_upper1, entity_data::val_t{});
		add_component_body(fore_upper2, entity_data::val_t{});
		add_component_body(fore_lower1, entity_data::val_t{});
		add_component_body(fore_lower2, entity_data::val_t{});
	}

	double test_quadruped_body::get_sensor_value(agent_sensor_id const& sensor) const
	{
		switch(sensor)
		{
			case spec::Sensors::RearLeftKneeAngle:
			return rear_knee1->GetJointAngle();
			case spec::Sensors::RearLeftKneeSpeed:
			return rear_knee1->GetJointSpeed();
			case spec::Sensors::RearRightKneeAngle:
			return rear_knee2->GetJointAngle();
			case spec::Sensors::RearRightKneeSpeed:
			return rear_knee2->GetJointSpeed();
			case spec::Sensors::RearLeftHipAngle:
			return rear_hip1->GetJointAngle();
			case spec::Sensors::RearLeftHipSpeed:
			return rear_hip1->GetJointSpeed();
			case spec::Sensors::RearRightHipAngle:
			return rear_hip2->GetJointAngle();
			case spec::Sensors::RearRightHipSpeed:
			return rear_hip2->GetJointSpeed();
			case spec::Sensors::ForeLeftKneeAngle:
			return fore_knee1->GetJointAngle();
			case spec::Sensors::ForeLeftKneeSpeed:
			return fore_knee1->GetJointSpeed();
			case spec::Sensors::ForeRightKneeAngle:
			return fore_knee2->GetJointAngle();
			case spec::Sensors::ForeRightKneeSpeed:
			return fore_knee2->GetJointSpeed();
			case spec::Sensors::ForeLeftHipAngle:
			return fore_hip1->GetJointAngle();
			case spec::Sensors::ForeLeftHipSpeed:
			return fore_hip1->GetJointSpeed();
			case spec::Sensors::ForeRightHipAngle:
			return fore_hip2->GetJointAngle();
			case spec::Sensors::ForeRightHipSpeed:
			return fore_hip2->GetJointSpeed();
			default:
			return composite_rigid_body::get_sensor_value(sensor);
		}
	}

	void test_quadruped_body::activate_effectors(std::vector< double > const& activations)
	{
		agent_body::activate_effectors(activations);

		float const MaxHipTorque = 100.0f;
		float const MaxKneeTorque = 60.0f;

		torso->ApplyTorque(-activations[0] * MaxHipTorque, true);
		rear_upper1->ApplyTorque(activations[0] * MaxHipTorque, true);
		torso->ApplyTorque(-activations[1] * MaxHipTorque, true);
		rear_upper2->ApplyTorque(activations[1] * MaxHipTorque, true);
		rear_upper1->ApplyTorque(-activations[2] * MaxKneeTorque, true);
		rear_lower1->ApplyTorque(activations[2] * MaxKneeTorque, true);
		rear_upper2->ApplyTorque(-activations[3] * MaxKneeTorque, true);
		rear_lower2->ApplyTorque(activations[3] * MaxKneeTorque, true);
		torso->ApplyTorque(-activations[4] * MaxHipTorque, true);
		fore_upper1->ApplyTorque(activations[4] * MaxHipTorque, true);
		torso->ApplyTorque(-activations[5] * MaxHipTorque, true);
		fore_upper2->ApplyTorque(activations[5] * MaxHipTorque, true);
		fore_upper1->ApplyTorque(-activations[6] * MaxKneeTorque, true);
		fore_lower1->ApplyTorque(activations[6] * MaxKneeTorque, true);
		fore_upper2->ApplyTorque(-activations[7] * MaxKneeTorque, true);
		fore_lower2->ApplyTorque(activations[7] * MaxKneeTorque, true);
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



