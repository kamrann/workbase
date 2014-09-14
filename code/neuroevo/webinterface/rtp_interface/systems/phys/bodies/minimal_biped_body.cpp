// minimal_biped_body.cpp

#include "minimal_biped_body.h"
#include "../rtp_phys_entity_data.h"
#include "../rtp_phys_system.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include "Box2D/Box2D.h"


namespace rtp {

	const double minimal_biped_body::spec::DefaultMaxKneeTorque = 75;
	const double minimal_biped_body::spec::DefaultMaxHipTorque = 75;

	namespace sb = prm::schema;

	std::string minimal_biped_body::spec::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		float const MaxHipTorque = 75.0f;
		float const MaxKneeTorque = 75.0f;

		auto relative = std::string{ "minimal_biped" };
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

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("max_torques");
			sb::append(s, provider->at(path + std::string("max_knee_torque"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_hip_torque"))(param_vals));
			sb::label(s, "Max Torques");
			return s;
		};

		path.pop();

		(*provider)[path + std::string("contact_damage")] = [=](prm::param_accessor)
		{
			auto s = sb::boolean("contact_damage", false);
			sb::label(s, "Contact Damage");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("max_torques"))(param_vals));
			sb::append(s, provider->at(path + std::string("contact_damage"))(param_vals));
			return s;
		};

		return relative;
	}

	minimal_biped_body::spec* minimal_biped_body::spec::create_instance(prm::param_accessor param)
	{
		param.push_relative_path(prm::qualified_path("minimal_biped"));
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		s->max_knee_torque = param["max_knee_torque"].as< double >();
		s->max_hip_torque = param["max_hip_torque"].as< double >();
		s->contact_damage = param["contact_damage"].as< bool >();
		param.pop_relative_path();
		return s;
	}

	agent_sensor_name_list minimal_biped_body::spec::sensor_inputs()
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
			std::string("Left Foot Contact"),
			std::string("Right Foot Contact"),
			std::string("Damage"),
			});
		return inputs;
	}

	size_t minimal_biped_body::spec::num_effectors()
	{
		return 4;
	}

	agent_body* minimal_biped_body::spec::create_body(b2World* world)
	{
		return nullptr;// new test_biped_body(*this, world);
	}

	minimal_biped_body::spec::spec(): agent_body_spec(agent_body_spec::Type::MinimalBiped)
	{}

	minimal_biped_body::minimal_biped_body(phys_agent_specification const& spec, phys_system* system) : composite_rigid_body(spec, system)
	{
		auto world = system->get_world();

		m_pelvis_height = 1.0f;
		m_pelvis_width = 1.0f;
		m_upper_len = 2.0f;
		m_upper_width = 0.5f;
		m_lower_len = 2.0f;
		m_lower_width = 0.4f;
		m_foot_radius = 0.4f;
		float32 const initial_y = m_pelvis_height / 2 + m_upper_len + m_lower_len + m_foot_radius + 0.1f;	// TODO: temp +0.1
		float32 const initial_hip_angle = 0;
		float32 const initial_knee_angle = 0;

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.angle = 0.0f;

		bd.position.Set(0.0f, initial_y);
		pelvis = world->CreateBody(&bd);

		bd.position.Set(0.0f, initial_y - m_pelvis_height / 2 - m_upper_len / 2);
		upper1 = world->CreateBody(&bd);
		upper2 = world->CreateBody(&bd);

		bd.position.Set(0.0f, initial_y - m_pelvis_height / 2 - m_upper_len - m_lower_len / 2);
		lower1 = world->CreateBody(&bd);
		lower2 = world->CreateBody(&bd);

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
		fd.friction = 0.5f;
		fd.filter.groupIndex = -1;

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
		// TODO: fix data value? body part enum?
		set_fixture_data(foot1, entity_fix_data{ entity_fix_data::Type::Other, entity_fix_data::val_t{ 0 } });
		set_fixture_data(foot2, entity_fix_data{ entity_fix_data::Type::Other, entity_fix_data::val_t{ 1 } });

		b2RevoluteJointDef jd;
		jd.enableLimit = true;
		jd.enableMotor = false;// true;
		jd.motorSpeed = -1.0f;
		jd.maxMotorTorque = 100.0f;
		jd.collideConnected = false;

		// Hip joints
		jd.lowerAngle = -b2_pi / 9;
		jd.upperAngle = b2_pi / 2;
		
		jd.Initialize(pelvis, upper1, b2Vec2(0.0f, initial_y - m_pelvis_height / 2));
		hip1 = (b2RevoluteJoint*)world->CreateJoint(&jd);
		
		jd.Initialize(pelvis, upper2, b2Vec2(0.0f, initial_y - m_pelvis_height / 2));
		hip2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		// Knee joints
		jd.lowerAngle = -3 * b2_pi / 4;
		jd.upperAngle = 0;
		
		jd.Initialize(upper1, lower1, b2Vec2(0.0f, initial_y - m_pelvis_height / 2 - m_upper_len));
		knee1 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		jd.Initialize(upper2, lower2, b2Vec2(0.0f, initial_y - m_pelvis_height / 2 - m_upper_len));
		knee2 = (b2RevoluteJoint*)world->CreateJoint(&jd);

		add_component_body(pelvis, entity_data::val_t{});
		add_component_body(upper1, entity_data::val_t{});
		add_component_body(upper2, entity_data::val_t{});
		add_component_body(lower1, entity_data::val_t{});
		add_component_body(lower2, entity_data::val_t{});

		m_max_knee_torque = spec.spec_acc["max_knee_torque"].as< double >();
		m_max_hip_torque = spec.spec_acc["max_hip_torque"].as< double >();
		m_contact_damage = spec.spec_acc["contact_damage"].as< bool >();

		left_foot_contact = false;
		right_foot_contact = false;

		damage = 0.0;
		life = 1.0;
	}

	double minimal_biped_body::get_sensor_value(agent_sensor_id const& sensor) const
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
			case spec::Sensors::LeftFootContact:
			return left_foot_contact ? 1.0 : 0.0;
			case spec::Sensors::RightFootContact:
			return right_foot_contact ? 1.0 : 0.0;
			case spec::Sensors::Damage:
			return damage;
			default:
			return composite_rigid_body::get_sensor_value(sensor);
		}
	}

	void minimal_biped_body::activate_effectors(std::vector< double > const& activations)
	{
		agent_body::activate_effectors(activations);

		pelvis->ApplyTorque(-activations[0] * m_max_hip_torque * life, true);
		upper1->ApplyTorque(activations[0] * m_max_hip_torque * life, true);
		pelvis->ApplyTorque(-activations[1] * m_max_hip_torque * life, true);
		upper2->ApplyTorque(activations[1] * m_max_hip_torque * life, true);
		upper1->ApplyTorque(-activations[2] * m_max_knee_torque * life, true);
		lower1->ApplyTorque(activations[2] * m_max_knee_torque * life, true);
		upper2->ApplyTorque(-activations[3] * m_max_knee_torque * life, true);
		lower2->ApplyTorque(activations[3] * m_max_knee_torque * life, true);
	}

	void minimal_biped_body::on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type)
	{
		// We know fixA is part of us, and also that we are not colliding limbs with each other, so fixB is external (assuming ground)
		auto fd = get_fixture_data(fixA);
		if(fd) // TODO: using existence of fixture data to imply foot
		{
			auto temp = boost::any_cast<int>(fd->value);
			switch(temp)
			{
				case 0:
				left_foot_contact = type == ContactType::Begin;
				break;

				case 1:
				right_foot_contact = type == ContactType::Begin;
				break;
			}
		}
		else if(type == ContactType::Begin)
		{
			// Non-foot contact
			damage += 1.0;

			if(m_contact_damage)
			{
				// For now, kill.
				life = 0.0;
			}
		}
	}
}



