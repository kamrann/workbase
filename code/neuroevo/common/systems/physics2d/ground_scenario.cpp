// ground_scenario.cpp

#include "ground_scenario.h"
#include "phys2d_system.h"


namespace sys {
	namespace phys2d {

		size_t ground_scenario::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			return 0;
		}

		ground_scenario::ground_scenario(ground_scenario_defn::spec_data spec):
			m_spec(std::move(spec)),
			m_ground(nullptr)
		{}

		bool ground_scenario::create(phys2d_system const* sys)
		{
			auto world = sys->get_world();

			world->SetGravity(b2Vec2(0.0f, -m_spec.gravity));

			auto const expanse = m_spec.expanse;

			if(expanse > 0.0)
			{
				b2BodyDef bd;
				m_ground = world->CreateBody(&bd);

				b2EdgeShape shape;
				shape.Set(
					b2Vec2(-(expanse / 2.0f) * std::cos(m_spec.incline), -(expanse / 2.0f) * std::sin(m_spec.incline)),
					b2Vec2((expanse / 2.0f) * std::cos(m_spec.incline), (expanse / 2.0f) * std::sin(m_spec.incline))
					);

				b2FixtureDef fd;
				fd.shape = &shape;
				fd.friction = m_spec.friction;

				m_ground->CreateFixture(&fd);
			}

			for(size_t i = 0; i < m_spec.ball_count; ++i)
			{
				auto const Radius = 0.1;

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(1.0 + i * 0.5, Radius);
				auto ball = world->CreateBody(&bd);

				b2CircleShape shape;
				shape.m_p.Set(0, 0);
				shape.m_radius = Radius;

				b2FixtureDef fd;
				fd.shape = &shape;
				fd.density = 1.0;
				fd.friction = 1.0;
				fd.restitution = 0.5;

				ball->CreateFixture(&fd);
			}

			return true;
		}

		bool ground_scenario::is_complete() const
		{
			return false;
		}

	}
}


