// test_body.cpp

#include "test_body.h"
#include "../rtp_phys_entity_data.h"
#include "../rtp_phys_system.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include "Box2D/Box2D.h"


namespace rtp {

	namespace sb = prm::schema;

	prm::schema::schema_node test_body::spec::get_schema(prm::param_accessor param_vals)
	{
		auto schema = sb::list("test_creature");
		sb::append(schema, sb::real("max_torque", 200.0));
		sb::label(sb::last(schema), "Max Joint Torque");
		return schema;
	}

	std::string test_body::spec::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "test_creature" };
		auto path = prefix + relative;

		(*provider)[path + std::string("max_torque")] = [](prm::param_accessor)
		{
			auto s = sb::real("max_torque", 200.0);
			sb::label(s, "Max Joint Torque");
			return s;
		};


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("max_torque"))(param_vals));
			return s;
		};

		return relative;
	}
	/*
	i_agent_factory* test_body::create_factory(prm::param_accessor spec_acc, prm::param_accessor inst_acc)
	{

	}
	*/

	test_body::spec* test_body::spec::create_instance(prm::param_accessor param)
	{
		param.push_relative_path(prm::qualified_path("test_creature"));
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		s->max_joint_torque = param["max_torque"].as< double >();
		param.pop_relative_path();
		return s;
	}

	test_body::spec* test_body::spec::create_instance(prm::param& param)
	{
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		s->max_joint_torque = param["max_torque"].as< double >();
		return s;
	}

	agent_sensor_name_list test_body::spec::sensor_inputs()
	{
		auto inputs = composite_rigid_body::sensor_inputs();
		inputs.insert(
			end(inputs),
			{
			std::string("Joint Angle"),
			std::string("Joint Speed"),
			});
		return inputs;
	}

	size_t test_body::spec::num_effectors()
	{
		return 1;
	}


	agent_body* test_body::spec::create_body(b2World* world)
	{
		return nullptr;// new test_body(*this, world);
	}

	test_body::spec::spec(): agent_body_spec(agent_body_spec::Type::TestCreature)
	{
	}


	test_body::test_body(phys_agent_specification const& spec, phys_system* system) : composite_rigid_body(spec, system)
	{
		auto world = system->get_world();

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

		add_component_body(rear, entity_data::val_t{});
		add_component_body(fore, entity_data::val_t{});

		m_max_joint_torque = spec.spec_acc["max_torque"].as< double >();
	}

	double test_body::get_sensor_value(agent_sensor_id const& sensor) const
	{
		switch(sensor)
		{
			case spec::Sensors::JointAngle:
			return joint->GetJointAngle();
			case spec::Sensors::JointSpeed:
			return joint->GetJointSpeed();
			default:
			return composite_rigid_body::get_sensor_value(sensor);
		}
	}

	void test_body::activate_effectors(std::vector< double > const& activations)
	{
		agent_body::activate_effectors(activations);

		auto torque = activations[0] * m_max_joint_torque;
		fore->ApplyTorque(torque, true);
		rear->ApplyTorque(-torque, true);
	}

	double test_body::get_max_joint_torque() const
	{
		return m_max_joint_torque;
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

