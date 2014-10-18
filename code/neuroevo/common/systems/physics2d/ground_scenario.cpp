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
			m_spec(std::move(spec))
		{}

		bool ground_scenario::create(phys2d_system const* sys)
		{
			auto world = sys->get_world();

			world->SetGravity(b2Vec2(0.0f, -9.81f));

			auto const expanse = m_spec.expanse;

			b2BodyDef bd;
			m_ground = world->CreateBody(&bd);

			b2EdgeShape shape;
			shape.Set(b2Vec2(-expanse / 2.0f, 0.0f), b2Vec2(expanse / 2.0f, 0.0f));

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.friction = 0.5f;	// TODO:

			m_ground->CreateFixture(&fd);

			return true;
		}

		bool ground_scenario::is_complete() const
		{
			return false;
		}

	}
}


