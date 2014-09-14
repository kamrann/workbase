// basic_spaceship.cpp

#include "basic_spaceship.h"

#include "../rtp_phys_system.h"
#include "../rtp_phys_sensors.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include "thrusters/thruster_presets.h"

#include <Wt/WPainter>

#include "Box2D/Box2D.h"


namespace rtp {

	namespace sb = prm::schema;

	prm::schema::schema_node basic_spaceship::spec::get_schema(prm::param_accessor param_vals)
	{
		auto schema = sb::list("spaceship");
		return schema;
	}

	std::string basic_spaceship::spec::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "spaceship" };
		auto path = prefix + relative;

		(*provider)[path + std::string("size")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"size",
				2.0,
				std::numeric_limits< double >::min()
				);
			sb::label(s, "Size");
			return s;
		};

		(*provider)[path + std::string("mass")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"mass",
				1.0,
				std::numeric_limits< double >::min()
				);
			sb::label(s, "Mass");
			return s;
		};

		(*provider)[path + std::string("rotational_inertia")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"rotational_inertia",
				1.0,
				std::numeric_limits< double >::min()
				);
			sb::label(s, "Rotational Inertia");
			return s;
		};

		(*provider)[path + std::string("thruster_strength")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"thruster_strength",
				1.0,
				0.0
				);
			sb::label(s, "Thruster Strength");
			return s;
		};

		(*provider)[path + std::string("sensor_range")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"sensor_range",
				10.0,
				0.0
				);
			sb::label(s, "Sensor Range");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("size"))(param_vals));
			sb::append(s, provider->at(path + std::string("mass"))(param_vals));
			sb::append(s, provider->at(path + std::string("rotational_inertia"))(param_vals));
			sb::append(s, provider->at(path + std::string("thruster_strength"))(param_vals));
			sb::append(s, provider->at(path + std::string("sensor_range"))(param_vals));
			return s;
		};

		return relative;
	}

	basic_spaceship::spec* basic_spaceship::spec::create_instance(prm::param_accessor param)
	{
		param.push_relative_path(prm::qualified_path("spaceship"));
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		param.pop_relative_path();
		return s;
	}

	basic_spaceship::spec* basic_spaceship::spec::create_instance(prm::param& param)
	{
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		return s;
	}

	agent_sensor_name_list basic_spaceship::spec::sensor_inputs()
	{
		auto inputs = simple_rigid_body::sensor_inputs();
		inputs.insert(
			end(inputs),
			{
			std::string("Front Sensor"),
			std::string("Rear Sensor"),
			std::string("Right Sensor"),
			std::string("Left Sensor"),
			std::string("Collisions"),
			std::string("Damage"),
			std::string("T1 Temp"),
			std::string("T2 Temp"),
			std::string("T3 Temp"),
			std::string("T4 Temp"),
			std::string("T5 Temp"),
			std::string("T6 Temp"),
			std::string("T7 Temp"),
			std::string("T8 Temp"),
			});
		return inputs;
	}

	size_t basic_spaceship::spec::num_effectors()
	{
		return 8;
	}


	agent_body* basic_spaceship::spec::create_body(b2World* world)
	{
		return nullptr;//  new basic_spaceship(*this, world);
	}

	basic_spaceship::spec::spec(): agent_body_spec(agent_body_spec::Type::Spaceship)
	{}

	basic_spaceship::basic_spaceship(phys_agent_specification const& spec, phys_system* system):
		simple_rigid_body(spec, system),
		m_t_cfg(new thruster_config< WorldDimensionality::dim2D >(thruster_presets::square_complete())),
		m_t_system(m_t_cfg)
	{
		m_half_width = spec.spec_acc["size"].as< double >() * 0.5;
		m_mass = spec.spec_acc["mass"].as< double >();
		m_rotational_inertia = spec.spec_acc["rotational_inertia"].as< double >();
		m_thruster_strength = spec.spec_acc["thruster_strength"].as< double >();
		m_sensor_range = spec.spec_acc["sensor_range"].as< double >();

		auto world = system->get_world();

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f, 0.0f);
		bd.angle = 0.0f;
		auto body = world->CreateBody(&bd);

		b2PolygonShape shape;
		shape.SetAsBox(m_half_width, m_half_width);

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.density = 1.0f;
		body->CreateFixture(&fd);

		// Override mass and rotational inertia
		// TODO: should be optional parameters, and if unspecified, left as auto b2d defaults
		b2MassData md;
		md.center = b2Vec2(0, 0);
		md.mass = m_mass;
		md.I = m_rotational_inertia;
		body->SetMassData(&md);

		set_body(body);

/*		phys_sensor_defn sdef(SensorType::Fan);
		sdef.body = body;
		sdef.pos = b2Vec2(0, 0);
		sdef.orientation = 0;
		sdef.range = m_sensor_range;
		sdef.type_specific = fan_sensor_data{ b2_pi / 3 };
		m_front_sensor = create_sensor(sdef);
		sdef.orientation = b2_pi;
		m_rear_sensor = create_sensor(sdef);
		sdef.orientation = b2_pi / 2;
		m_left_sensor = create_sensor(sdef);
		sdef.orientation = -b2_pi / 2;
		m_right_sensor = create_sensor(sdef);
*/

		phys_sensor_defn sdef(SensorType::Directional);
		sdef.body = body;
		sdef.pos = b2Vec2(0, 0);
		sdef.orientation = 0;
		sdef.range = m_sensor_range;
		sdef.type_specific = directional_sensor_data{};
		m_front_sensor = create_sensor(sdef);
		sdef.orientation = b2_pi;
		m_rear_sensor = create_sensor(sdef);
		sdef.orientation = b2_pi / 2;
		m_left_sensor = create_sensor(sdef);
		sdef.orientation = -b2_pi / 2;
		m_right_sensor = create_sensor(sdef);

		m_collisions = 0;
		m_damage = 0.0;
	}

	double basic_spaceship::get_sensor_value(agent_sensor_id const& sensor) const
	{
		switch(sensor)
		{
			case spec::Sensors::FrontSensor:
			return m_front_sensor->read();
			case spec::Sensors::RearSensor:
			return m_rear_sensor->read();
			case spec::Sensors::RightSensor:
			return m_right_sensor->read();
			case spec::Sensors::LeftSensor:
			return m_left_sensor->read();
			case spec::Sensors::Collisions:
			return m_collisions;
			case spec::Sensors::Damage:
			return m_damage;
			case spec::Sensors::Thruster_1_Temp:
			return m_t_system[0].t.temperature();
			case spec::Sensors::Thruster_2_Temp:
			return m_t_system[1].t.temperature();
			case spec::Sensors::Thruster_3_Temp:
			return m_t_system[2].t.temperature();
			case spec::Sensors::Thruster_4_Temp:
			return m_t_system[3].t.temperature();
			case spec::Sensors::Thruster_5_Temp:
			return m_t_system[4].t.temperature();
			case spec::Sensors::Thruster_6_Temp:
			return m_t_system[5].t.temperature();
			case spec::Sensors::Thruster_7_Temp:
			return m_t_system[6].t.temperature();
			case spec::Sensors::Thruster_8_Temp:
			return m_t_system[7].t.temperature();

			default:
			return simple_rigid_body::get_sensor_value(sensor);
		}
	}

	void basic_spaceship::activate_effectors(std::vector< double > const& activations)
	{
		agent_body::activate_effectors(activations);

		thruster_base::thruster_activation ta(activations.size());
		std::transform(begin(activations), end(activations), begin(ta), [](double d_act) {
			return d_act > 0.0;
		});

		for(size_t i = 0; i < m_t_cfg->num_thrusters(); ++i)
		{
			if(ta[i])
			{
				b2Vec2 pos = m_half_width * b2Vec2((*m_t_cfg)[i].pos[0], (*m_t_cfg)[i].pos[1]);
				b2Vec2 dir((*m_t_cfg)[i].dir[0], (*m_t_cfg)[i].dir[1]);
				apply_force_local(m_thruster_strength * dir, pos);

				m_t_system[i].t.engage();
			}
			else
			{
				m_t_system[i].t.cool_down(1.0f / 60.0f);	// TODO: Need to know internal system update frequency
			}
		}
	}

	void basic_spaceship::on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type)
	{
		if(type == ContactType::Begin)
		{
			++m_collisions;
		}
	}

	void basic_spaceship::on_collision(b2Fixture* fixA, b2Fixture* fixB, double approach_speed)
	{
		m_damage += approach_speed;
	}

	void basic_spaceship::draw(Wt::WPainter& painter) const
	{
		simple_rigid_body::draw(painter);

/*		Wt::WRectF rc(
			-m_half_width,
			-m_half_width,
			m_half_width * 2,
			m_half_width * 2
			);
*/		painter.save();
		painter.translate(m_body->GetPosition().x, m_body->GetPosition().y);
		painter.rotate(m_body->GetAngle() * 360.0 / (2 * b2_pi));
//		painter.drawRect(rc);

		thruster_config< WorldDimensionality::dim2D > const& t_cfg = *m_t_cfg;
		double const ThrusterSize = m_half_width * 0.3;
		std::array< Wt::WPointF, 3 > points = {
			Wt::WPointF(0, 0),
			Wt::WPointF(-ThrusterSize / 2, -ThrusterSize),
			Wt::WPointF(ThrusterSize / 2, -ThrusterSize)
		};
		Wt::WPen pen(Wt::GlobalColor::black);
		for(size_t i = 0; i < m_t_cfg->num_thrusters(); ++i)
		{
			painter.save();

			auto const& tpos = m_half_width * t_cfg[i].pos;
			painter.translate(tpos[0], tpos[1]);
			double dir_x = t_cfg[i].dir[0];
			double dir_y = t_cfg[i].dir[1];
			double theta = boost::math::copysign(std::acos(dir_y), dir_x);
			painter.rotate(360.0 * -theta / (2 * boost::math::double_constants::pi));

			double temperature = m_t_system[i].t.temperature();
			Wt::WColor color((int)(temperature * 255), 0, 0);
			pen.setColor(color);
			painter.setPen(pen);
			Wt::WBrush br(color);
			painter.setBrush(br);
			painter.drawPolygon(&points[0], points.size());

			painter.restore();
		}

//		double const IndicatorSize = m_half_width * 0.35;
		auto const IndicatorPos = b2Vec2(0.f, 1.4f * m_half_width);
		m_front_sensor->draw(painter, IndicatorPos);
		m_rear_sensor->draw(painter, IndicatorPos);
		m_left_sensor->draw(painter, IndicatorPos);
		m_right_sensor->draw(painter, IndicatorPos);

		painter.restore();
	}
}



