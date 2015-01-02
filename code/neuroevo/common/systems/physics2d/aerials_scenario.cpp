// aerials_scenario.cpp

#include "aerials_scenario.h"
#include "phys2d_system.h"

//
#include <iostream>
//


namespace sys {
	namespace phys2d {

		size_t aerials_scenario::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			return 0;
		}

		aerials_scenario::aerials_scenario(aerials_scenario_defn::spec_data spec):
			m_spec(std::move(spec)),
			m_ground(nullptr)
		{}

		bool aerials_scenario::create(phys2d_system const* sys)
		{
			auto world = sys->get_world();

			world->SetGravity(b2Vec2(0.0f, -m_spec.gravity));

			b2BodyDef bd;
			m_ground = world->CreateBody(&bd);

			b2ChainShape shape;
			std::vector< b2Vec2 > verts;

			// Platform
			auto const PlatformWidth = m_spec.platform_width;
			auto const PlatformWallHeight = m_spec.wall_height;
			verts.emplace_back(-PlatformWidth / 2.0, PlatformWallHeight);
			verts.emplace_back(-PlatformWidth / 2.0, 0.0);
			verts.emplace_back(PlatformWidth / 2.0, 0.0);

			// Initial straight slope
			auto const InitialSlopeGradient = -m_spec.slope_gradient;
			auto const InitialSlopeDrop = -m_spec.slope_drop;
			verts.emplace_back(verts.back().x + InitialSlopeDrop / InitialSlopeGradient, verts.back().y + InitialSlopeDrop);

			// Quadratic first dip
			// y = x^n
			// Match the gradient - dy/dx = nx
			auto const n = 2.0;
			auto x1 = InitialSlopeGradient / n;
			auto const FirstDipDepth = m_spec.dip_depth;
			auto y1 = x1 * x1;
			auto scale = FirstDipDepth / y1;
			auto const FirstDipPoints = 30;
			auto base = verts.back();
			auto const RampGradient = m_spec.ramp_gradient;
			auto x2 = RampGradient / n;
			for(size_t i = 1; i <= FirstDipPoints; ++i)
			{
				auto x = x1 + ((x2 - x1) * i) / FirstDipPoints;
				auto y = x * x;

				x = x - x1;
				y = y - y1;
				x *= scale;
				y *= scale;

				verts.emplace_back(base.x + x, base.y + y);
			}

			auto const RampWidth = m_spec.ramp_width;
			verts.emplace_back(verts.back().x + RampWidth, verts.back().y);

			// Cycloid curve for landing slope
			auto const LandingSlopeDrop = m_spec.landing_slope_drop;
			auto const LandingSlopePoints = 30;
			base = verts.back();
			for(size_t i = 1; i <= LandingSlopePoints; ++i)
			{
				auto t = M_PI * i / LandingSlopePoints;
				auto r = LandingSlopeDrop / 2.0;
				auto x = r * (t - std::sin(t));
				auto y = -r * (1.0 - std::cos(t));

				verts.emplace_back(base.x + x, base.y + y);
			}

			auto const FlatGroundWidth = m_spec.landing_flat_width;
			auto const EndWallHeight = m_spec.wall_height;
			verts.emplace_back(verts.back().x + FlatGroundWidth, verts.back().y);
			verts.emplace_back(verts.back().x, verts.back().y + EndWallHeight);

			for(auto& v : verts)
			{
				v *= m_spec.overall_scale;
			}
			shape.CreateChain(verts.data(), verts.size());
#if 0
			std::cout << "Terrain vertices:" << std::endl;
			for(auto const& v : verts)
			{
				std::cout << v.x << ", " << v.y << std::endl;
			}
#endif
			b2FixtureDef fd;
			fd.shape = &shape;
			fd.friction = m_spec.surface_friction;
			//
//			fd.restitution = 0.7f;
			//

			m_ground->CreateFixture(&fd);

			return true;
		}

		bool aerials_scenario::is_complete() const
		{
			return false;
		}

	}
}


