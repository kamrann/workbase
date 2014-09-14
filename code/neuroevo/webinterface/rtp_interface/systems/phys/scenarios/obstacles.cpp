// obstacles.cpp

#include "obstacles.h"

#include "../b2d_util.h"

#include "wt_param_widgets/param_accessor.h"
#include "wt_param_widgets/schema_builder.h"

//
#include <Wt/WPainter>
//


namespace rtp {

	namespace sb = prm::schema;

	std::string obstacles_scenario::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "obstacles" };
		auto path = prefix + relative;

		(*provider)[path + std::string("x_expanse")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"x_expanse",
				10.0,
				0.0
				);
			sb::label(s, "X Expanse");
			return s;
		};

		(*provider)[path + std::string("y_expanse")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"y_expanse",
				10.0,
				0.0
				);
			sb::label(s, "Y Expanse");
			return s;
		};

		(*provider)[path + std::string("x_center")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"x_center",
				0.0
				);
			sb::label(s, "X Center");
			return s;
		};

		(*provider)[path + std::string("y_center")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"y_center",
				0.0
				);
			sb::label(s, "Y Center");
			return s;
		};

		(*provider)[path + std::string("density")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"density",
				0.1,
				0.0
				);
			sb::label(s, "Density");
			return s;
		};

		(*provider)[path + std::string("min_size")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"min_size",
				1.0,
				0.0
				);
			sb::label(s, "Min Size");
			return s;
		};

		(*provider)[path + std::string("max_size")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"max_size",
				1.0,
				0.0
				);
			sb::label(s, "Max Size");
			return s;
		};

		(*provider)[path + std::string("max_elongation")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"max_elongation",
				1.0,
				1.0
				);
			sb::label(s, "Max Elongation");
			return s;
		};

		(*provider)[path + std::string("max_speed")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"max_speed",
				10.0,
				0.0
				);
			sb::label(s, "Max Speed");
			return s;
		};

		(*provider)[path + std::string("max_angular_speed")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"max_angular_speed",
				10.0,
				0.0
				);
			sb::label(s, "Max Angular Speed");
			return s;
		};

		(*provider)[path + std::string("freezone_x")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"freezone_x",
				0.0
				);
			sb::label(s, "Free Zone X");
			return s;
		};

		(*provider)[path + std::string("freezone_y")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"freezone_y",
				0.0
				);
			sb::label(s, "Free Zone Y");
			return s;
		};

		(*provider)[path + std::string("freezone_radius")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"freezone_radius",
				0.0,
				0.0
				);
			sb::label(s, "Free Zone Radius");
			return s;
		};


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("x_expanse"))(param_vals));
			sb::append(s, provider->at(path + std::string("y_expanse"))(param_vals));
			sb::append(s, provider->at(path + std::string("x_center"))(param_vals));
			sb::append(s, provider->at(path + std::string("y_center"))(param_vals));
			sb::append(s, provider->at(path + std::string("density"))(param_vals));
			sb::append(s, provider->at(path + std::string("min_size"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_size"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_elongation"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_speed"))(param_vals));
			sb::append(s, provider->at(path + std::string("max_angular_speed"))(param_vals));
			sb::append(s, provider->at(path + std::string("freezone_x"))(param_vals));
			sb::append(s, provider->at(path + std::string("freezone_y"))(param_vals));
			sb::append(s, provider->at(path + std::string("freezone_radius"))(param_vals));
			return s;
		};

		return relative;
	}

	std::vector< std::string > obstacles_scenario::get_state_values(prm::param_accessor param_vals)
	{
		return{
//			"Collision Count",
		};
	}


	obstacles_scenario::obstacles_scenario(prm::param_accessor param_vals)
	{
		m_x_expanse = param_vals["x_expanse"].as< double >();
		m_y_expanse = param_vals["y_expanse"].as< double >();
		m_x_center = param_vals["x_center"].as< double >();
		m_y_center = param_vals["y_center"].as< double >();
		m_density = param_vals["density"].as< double >();
		m_min_size = param_vals["min_size"].as< double >();
		m_max_size = param_vals["max_size"].as< double >();
		m_max_elongation = param_vals["max_elongation"].as< double >();
		m_max_speed = param_vals["max_speed"].as< double >();
		m_max_angular_speed = param_vals["max_angular_speed"].as< double >();
		m_freezone_x = param_vals["freezone_x"].as< double >();
		m_freezone_y = param_vals["freezone_y"].as< double >();
		m_freezone_radius = param_vals["freezone_radius"].as< double >();
	}

	boost::any obstacles_scenario::generate_initial_state(rgen_t& rgen) const
	{
		typedef DimensionalityTraits< WorldDimensionality::dim2D > dim_traits_t;

		init_state_data data;

		auto const area = m_x_expanse * m_y_expanse;
		size_t const count = area * m_density;

		boost::random::uniform_real_distribution<> dist(-1.0, 1.0);
		for(size_t i = 0; i < count; ++i)
		{
			init_state_data::obstacle ob;
			ob.pos.x = m_x_center + dist(rgen) * m_x_expanse / 2;
			ob.pos.y = m_y_center + dist(rgen) * m_y_expanse / 2;

			if((ob.pos - b2Vec2(m_freezone_x, m_freezone_y)).Length() < m_freezone_radius)
			{
				continue;	// TODO: Maintain density outside of freezone??
			}

			ob.angle = dist(rgen) * b2_pi;
			auto vel = random_val< dim_traits_t::velocity_t >(rgen) * m_max_speed;
			ob.vel = b2Vec2(vel[0], vel[1]);
			ob.angvel = random_val< dim_traits_t::angular_velocity_t >(rgen) * m_max_angular_speed;
			
			double const min_angle_step = b2_pi / 4;
			double const max_angle_step = b2_pi / 2;
			auto radius = boost::random::uniform_real_distribution<>(m_min_size, m_max_size)(rgen);
			auto elongation = boost::random::uniform_real_distribution<>(1.0 / m_max_elongation, 1.0)(rgen);
			double last_angle;
			do
			{
				double angle;
				for(
					ob.vertex_count = 0, angle = 0.0;
					ob.vertex_count < b2_maxPolygonVertices && angle < 2.0 * b2_pi;
				)
				{
					ob.vertices[ob.vertex_count++] = b2Vec2(
						radius * std::cos(angle) * elongation,
						radius * std::sin(angle));
					last_angle = angle;
					angle += boost::random::uniform_real_distribution<>(min_angle_step, max_angle_step)(rgen);
				}

			} while(last_angle < 2.0 * b2_pi - max_angle_step);

			data.obstacles.emplace_back(std::move(ob));
		}

		return boost::any(data);
	}

	void obstacles_scenario::load_initial_state(boost::any const& data, b2World* world)
	{
		auto init_data = boost::any_cast<init_state_data>(data);

		world->SetGravity(b2Vec2(0.0f, 0.0f));

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		b2PolygonShape poly;
		b2FixtureDef fd;
		fd.shape = &poly;
		fd.density = 1.0f;
		for(auto const& ob : init_data.obstacles)
		{
			auto body = world->CreateBody(&bd);
			//body->SetUserData(reinterpret_cast<void*>(ObstacleId));
			poly.Set(&ob.vertices[0], ob.vertex_count);
			body->CreateFixture(&fd);
			body->SetTransform(ob.pos, ob.angle);
			body->SetLinearVelocity(ob.vel);
			body->SetAngularVelocity(ob.angvel);
			m_obstacles.push_back(body);
		}

//		m_collisions = 0;
	}

	bool obstacles_scenario::is_complete(state_t const& st)
	{
		return st.time >= m_duration;// || m_collisions > 0;
	}
/*
	void obstacles_scenario::BeginContact(b2Contact* contact)
	{
		auto fixA = contact->GetFixtureA();
		auto fixB = contact->GetFixtureB();
		auto bodyA = fixA->GetBody();
		auto bodyB = fixB->GetBody();

		bool A_is_obstacle = reinterpret_cast<unsigned long>(bodyA->GetUserData()) == ObstacleId;
		bool B_is_obstacle = reinterpret_cast<unsigned long>(bodyB->GetUserData()) == ObstacleId;
		// TODO: For now just assuming that anything not an obstacle is the single agent body
		bool agent_involved = !A_is_obstacle || !B_is_obstacle;
		if(agent_involved)
		{
			auto agent_fix = A_is_obstacle ? fixB : fixA;
			auto ud = agent_fix->GetUserData();

			// TODO:
			if(ud == nullptr)
			{
				++m_collisions;
			}
			else
			{
				// assume sensor
				auto sen = static_cast<phys_sensor*>(ud);
				sen->update(phys_sensor::UpdateType::Begin);
			}
		}
	}

	void obstacles_scenario::EndContact(b2Contact* contact)
	{
		auto fixA = contact->GetFixtureA();
		auto fixB = contact->GetFixtureB();
		auto bodyA = fixA->GetBody();
		auto bodyB = fixB->GetBody();

		bool A_is_obstacle = reinterpret_cast<unsigned long>(bodyA->GetUserData()) == ObstacleId;
		bool B_is_obstacle = reinterpret_cast<unsigned long>(bodyB->GetUserData()) == ObstacleId;
		// TODO: For now just assuming that anything not an obstacle is the single agent body
		bool agent_involved = !A_is_obstacle || !B_is_obstacle;
		if(agent_involved)
		{
			auto agent_fix = A_is_obstacle ? fixB : fixA;
			auto ud = agent_fix->GetUserData();

			// TODO:
			if(ud != nullptr)
			{
				// assume sensor
				auto sen = static_cast<phys_sensor*>(ud);
				sen->update(phys_sensor::UpdateType::End);
			}
		}
	}
*/
	void obstacles_scenario::draw_fixed_objects(Wt::WPainter& painter) const
	{
		for(auto body : m_obstacles)
		{
			draw_body(body, painter);
		}
	}

	double obstacles_scenario::get_state_value(state_value_id id) const
	{
/*		if(id.compare("Collision Count") == 0)
		{
			return m_collisions;
		}
		else
*/		{
			return 0.0;
		}
	}
}


