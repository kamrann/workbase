// target.cpp

#include "target.h"

#include "../b2d_util.h"

#include "wt_param_widgets/param_accessor.h"
#include "wt_param_widgets/schema_builder.h"

//
#include <Wt/WPainter>
//


namespace rtp {

	namespace sb = prm::schema;

	std::string target_scenario::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "target" };
		auto path = prefix + relative;

		(*provider)[path + std::string("target_distance")] = [](prm::param_accessor)
		{
			auto s = sb::random(
				"target_distance",
				5.0,
				0.0,
				boost::none
				);
			sb::label(s, "Start Distance");
			return s;
		};


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("target_distance"))(param_vals));
			return s;
		};

		return relative;
	}


	target_scenario::target_scenario(prm::param_accessor param_vals)
	{
		m_target_start_distance = prm::extract_fixed_or_random(param_vals["target_distance"]);
	}

	boost::any target_scenario::generate_initial_state(rgen_t& rgen) const
	{
		init_state_data data;
		auto target_start_dist = m_target_start_distance.get(rgen);
		typedef DimensionalityTraits< WorldDimensionality::dim2D > dim_traits_t;
		auto pos = random_dir_val< dim_traits_t::direction_t, rgen_t >(rgen) * target_start_dist;
		data.target_pos.x = pos[0];
		data.target_pos.y = pos[1];

		size_t const count = 20;
		b2Vec2 const obstacles_center(0.0f, 20.0f);
		double const x_obstacle_boundary = 20.0;
		double const y_obstacle_boundary = 10.0;
		double const max_obstacle_speed = 10.0;
		double const max_obstacle_angular_speed = 10.0;
		double const min_obstacle_size = 1.0;
		double const max_obstacle_size = 4.0;
		double const max_elongation = 3.0;

		boost::random::uniform_real_distribution<> dist(-1.0, 1.0);
		for(size_t i = 0; i < count; ++i)
		{
			init_state_data::obstacle ob;
			ob.pos.x = obstacles_center.x + dist(rgen) * x_obstacle_boundary;
			ob.pos.y = obstacles_center.y + dist(rgen) * y_obstacle_boundary;
			ob.angle = dist(rgen) * b2_pi;
			auto vel = random_val< dim_traits_t::velocity_t >(rgen) * max_obstacle_speed;
			ob.vel = b2Vec2(vel[0], vel[1]);
			ob.angvel = random_val< dim_traits_t::angular_velocity_t >(rgen) * max_obstacle_angular_speed;
			
			double const min_angle_step = b2_pi / 4;
			double const max_angle_step = b2_pi / 2;
			auto radius = boost::random::uniform_real_distribution<>(min_obstacle_size, max_obstacle_size)(rgen);
			auto elongation = boost::random::uniform_real_distribution<>(1.0 / max_elongation, 1.0)(rgen);
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

	void target_scenario::load_initial_state(boost::any const& data, b2World* world)
	{
		auto init_data = boost::any_cast<init_state_data>(data);

		world->SetGravity(b2Vec2(0.0f, 0.0f));

		b2BodyDef bd;
		bd.type = b2_staticBody;
//		m_target = world->CreateBody(&bd);

		b2CircleShape shape;
		shape.m_p = init_data.target_pos;
		shape.m_radius = 1.0f;

		b2FixtureDef fd;
		fd.shape = &shape;
//		m_target->CreateFixture(&fd);

		bd.type = b2_dynamicBody;
		b2PolygonShape poly;
		fd.shape = &poly;
		fd.density = 1.0f;
		for(auto const& ob : init_data.obstacles)
		{
			auto body = world->CreateBody(&bd);
			body->SetUserData(reinterpret_cast<void*>(ObstacleId));
			poly.Set(&ob.vertices[0], ob.vertices.size());
			body->CreateFixture(&fd);
			body->SetTransform(ob.pos, ob.angle);
			body->SetLinearVelocity(ob.vel);
			body->SetAngularVelocity(ob.angvel);
			m_obstacles.push_back(body);
		}

		m_collided = false;
	}

	bool target_scenario::is_complete(state_t const& st)
	{
		return st.time >= m_duration || m_collided;
	}

	void target_scenario::BeginContact(b2Contact* contact)
	{
		auto fixA = contact->GetFixtureA();
		auto fixB = contact->GetFixtureB();
		auto bodyA = fixA->GetBody();
		auto bodyB = fixB->GetBody();

		if(reinterpret_cast<unsigned long>(bodyA->GetUserData()) != ObstacleId ||
			reinterpret_cast<unsigned long>(bodyB->GetUserData()) != ObstacleId)
		{
			// TODO: For now just assuming that anything not an obstacle is the single agent body
			m_collided = true;
		}
	}

	void target_scenario::EndContact(b2Contact* contact)
	{

	}

	void target_scenario::draw_fixed_objects(Wt::WPainter& painter) const
	{
		//draw_body(m_target, painter);

		for(auto body : m_obstacles)
		{
			draw_body(body, painter);
		}
	}
}


