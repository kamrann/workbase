// ski_jump_scenario.cpp

#include "ski_jump_scenario.h"
#include "phys2d_system.h"

#include "b2d_components/core/entity_data.h"

//
#include <iostream>
//


namespace sys {
	namespace phys2d {

		size_t ski_jump_scenario::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			auto initial_count = bindings.size();
			auto bound_id = accessors.size();
			state_value_id sv_id;

			sv_id = state_value_id::from_string("jump_distance");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return m_landing_contact_min_x ? (*m_landing_contact_min_x - m_landing_x_start) : 0.0;
			});

			sv_id = state_value_id::from_string("on_platform");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return on_platform() ? 1.0 : 0.0;
			});

			sv_id = state_value_id::from_string("platform_pos");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return platform_position();
			});

			sv_id = state_value_id::from_string("on_takeoff");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return on_takeoff() ? 1.0 : 0.0;
			});

			sv_id = state_value_id::from_string("takeoff_pos");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return takeoff_position();
			});

			return bindings.size() - initial_count;
		}

		ski_jump_scenario::ski_jump_scenario(ski_jump_scenario_defn::spec_data spec):
			m_spec(std::move(spec)),
			m_ground(nullptr)
		{}

		bool ski_jump_scenario::create(phys2d_system const* sys)
		{
			auto world = sys->get_world();

			world->SetGravity(b2Vec2(0.0f, -m_spec.gravity));

			b2BodyDef bd;
			m_ground = world->CreateBody(&bd);

			b2ChainShape shape;
			std::vector< b2Vec2 > verts;
			std::vector< size_t > chain_indices;
			chain_indices.push_back(0);

			// Platform
			auto const PlatformWidth = m_spec.platform_width;
			auto const PlatformWallHeight = m_spec.wall_height;
// todo:			verts.emplace_back(-PlatformWidth / 2.0, PlatformWallHeight);
			verts.emplace_back(-PlatformWidth / 2.0, 0.0);
			m_platform_line[0] = verts.back();
			verts.emplace_back(PlatformWidth / 2.0, 0.0);
			m_platform_line[1] = verts.back();

			///
			verts.emplace_back(verts.back().x + PlatformWidth / 4, verts.back().y - PlatformWidth / 8);
			///

			chain_indices.push_back(verts.size());

			/*
			Create sensors to pick up if and where the agent is on the platform.
			TODO: Really scenario needs access to system agent(s)/roles.
			*/
			auto const NumPlatformSensors = 5;
			auto const SensorWidth = PlatformWidth / NumPlatformSensors;
			auto const SensorHeight = 1.0;
			for(size_t i = 0; i < NumPlatformSensors; ++i)
			{
				b2PolygonShape sen_shape;
				sen_shape.SetAsBox(SensorWidth / 2, SensorHeight / 2,
					b2Vec2(-PlatformWidth / 2 + (i + 0.5) * SensorWidth, 0.0), 0.0);

				b2FixtureDef fd;
				fd.isSensor = true;
				fd.shape = &sen_shape;

				auto fix = m_ground->CreateFixture(&fd);
				auto tsd = terrain_sensor_data{ terrain_sensor_data::Platform, i };
				b2dc::set_fixture_data(fix, b2dc::entity_fix_data{ b2dc::entity_fix_data::Type::Other, b2dc::entity_fix_data::val_t{ tsd } });
			}
			m_platform_sensors.clear();
			m_platform_sensors.resize(NumPlatformSensors, false);

			// Initial straight slope
			auto const InitialSlopeIncline = m_spec.inrun_degrees;
			auto const InitialSlopeLength = m_spec.inrun_length;
			verts.emplace_back(
				verts.back().x + InitialSlopeLength * std::cos(InitialSlopeIncline),
				verts.back().y - InitialSlopeLength * std::sin(InitialSlopeIncline)
				);

			chain_indices.push_back(verts.size());

			// Circular transition
			auto const TakeoffIncline = m_spec.takeoff_degrees;
			auto const r1 = m_spec.transition1_radius;
			auto c1 = verts.back() + b2Vec2(
				r1 * std::sin(InitialSlopeIncline),
				r1 * std::cos(InitialSlopeIncline)
				);

			double const t1_angle = TakeoffIncline - InitialSlopeIncline;	// expect to be negative
			auto const Transition1Points = 20;
			auto base = verts.back() - c1;
			for(size_t i = 1; i <= Transition1Points; ++i)
			{
				auto prop = t1_angle * i / Transition1Points;
				verts.push_back(c1 + b2Mul(b2Rot(-prop), base));
			}

			chain_indices.push_back(verts.size());

			m_takeoff_line[0] = verts.back();
			auto const TakeoffLength = m_spec.takeoff_length;
			verts.emplace_back(
				verts.back().x + TakeoffLength * std::cos(TakeoffIncline),
				verts.back().y - TakeoffLength * std::sin(TakeoffIncline)
				);
			m_takeoff_line[1] = verts.back();

			chain_indices.push_back(verts.size());

			auto const NumTakeoffSensors = 5;
			auto const TO_SensorWidth = TakeoffLength / NumTakeoffSensors;
			auto const TO_SensorHeight = 1.0;
			for(size_t i = 0; i < NumTakeoffSensors; ++i)
			{
				b2PolygonShape sen_shape;
				b2Vec2 offset = m_takeoff_line[1] - m_takeoff_line[0];
				offset *= (i + 0.5) / TakeoffLength;
				sen_shape.SetAsBox(TO_SensorWidth / 2, TO_SensorHeight / 2,
					m_takeoff_line[0] + offset,
					0.0 /* TODO: if necessary */);

				b2FixtureDef fd;
				fd.isSensor = true;
				fd.shape = &sen_shape;

				auto fix = m_ground->CreateFixture(&fd);
				auto tsd = terrain_sensor_data{ terrain_sensor_data::Takeoff, i };
				b2dc::set_fixture_data(fix, b2dc::entity_fix_data{ b2dc::entity_fix_data::Type::Other, b2dc::entity_fix_data::val_t{ tsd } });
			}
			m_takeoff_sensors.clear();
			m_takeoff_sensors.resize(NumTakeoffSensors, false);

			verts.emplace_back(
				verts.back().x,
				verts.back().y - m_spec.jump_dropoff
				);

			chain_indices.push_back(verts.size());

			m_landing_x_start = verts.back().x;

			// Use sigmoid for landing, transition 2 and outrun area
			// y = 1 / (1 + e^-x)
			auto const x1 = m_spec.landing_x1;
			auto const x2 = m_spec.landing_x2;
			auto const y1 = 1.0 / (1.0 + std::exp(-x1));

			base = verts.back();
			auto const LandingPoints = 40;
			for(size_t i = 1; i <= LandingPoints; ++i)
			{
				auto x_prop = (double)i / LandingPoints;
				auto x = x1 + (x2 - x1) * x_prop;
				auto y = 1.0 / (1.0 + std::exp(-x));

				verts.emplace_back(
					base.x + x_prop * m_spec.landing_length,
					base.y - (1.0 - y / y1) * m_spec.landing_drop
					);
			}

			chain_indices.push_back(verts.size());

			// TODO: Slight upcurve at end?

			verts.emplace_back(
				verts.back().x,
				verts.back().y + m_spec.wall_height
				);

			chain_indices.push_back(verts.size());


			b2FixtureDef fd;
			fd.shape = &shape;
			fd.friction = m_spec.surface_friction;

			for(size_t c = 0; c < chain_indices.size() - 1; ++c)
			{
				auto first = (c == 0 ? chain_indices[c] : chain_indices[c - 1]);
				auto count = chain_indices[c + 1] - first;

				shape.Clear();
				shape.CreateChain(verts.data() + first, count);

				if(first != 0)
				{
					shape.SetPrevVertex(verts[first - 1]);
				}
				if(first + count < verts.size())
				{
					shape.SetNextVertex(verts[first + count]);
				}

				m_ground->CreateFixture(&fd);
			}

#if 0
			std::cout << "Terrain vertices:" << std::endl;
			for(auto const& v : verts)
			{
				std::cout << v.x << ", " << v.y << std::endl;
			}
#endif

			b2dc::entity_data ed{};
			ed.type = b2dc::entity_data::Type::Scenario;
			ed.type_value = ScenarioComponent::Terrain;
//			ed.value = std::move(data);
			set_body_data(m_ground, std::move(ed));

			m_landing_contact_min_x = boost::none;

			return true;
		}

		void ski_jump_scenario::on_contact(b2Contact* contact, b2dc::ContactType ctype)
		{
			auto fixA = contact->GetFixtureA();
			auto fixB = contact->GetFixtureB();

			auto bodyA = fixA->GetBody();
			auto bodyB = fixB->GetBody();

			auto edA = b2dc::get_body_data(bodyA);
			auto edB = b2dc::get_body_data(bodyB);

			auto const num_points = contact->GetManifold()->pointCount;

			b2Fixture* scenario_fix = nullptr;
			b2Fixture* agent_fix = nullptr;
			if(
				edA->type == b2dc::entity_data::Type::Scenario
				&& boost::any_cast<ScenarioComponent>(edA->type_value) == ScenarioComponent::Terrain
				&& edB->type == b2dc::entity_data::Type::Agent
				)
			{
				scenario_fix = fixA;
				agent_fix = fixB;
			}
			else if(
				edB->type == b2dc::entity_data::Type::Scenario
				&& boost::any_cast<ScenarioComponent>(edB->type_value) == ScenarioComponent::Terrain
				&& edA->type == b2dc::entity_data::Type::Agent
				)
			{
				scenario_fix = fixB;
				agent_fix = fixA;
			}

			if(!scenario_fix)
			{
				return;
			}

			auto efd = b2dc::get_fixture_data(scenario_fix);
			if(efd)
			{
				auto const& tsd = boost::any_cast<terrain_sensor_data const&>(efd->value);
				switch(tsd.type)
				{
					case terrain_sensor_data::Platform:
					if(ctype == b2dc::ContactType::Begin)
					{
						m_platform_sensors[tsd.index] += 1;
					}
					else
					{
						m_platform_sensors[tsd.index] -= 1;
					}
					break;

					case terrain_sensor_data::Takeoff:
					if(ctype == b2dc::ContactType::Begin)
					{
						m_takeoff_sensors[tsd.index] += 1;
					}
					else
					{
						m_takeoff_sensors[tsd.index] -= 1;
					}
					break;
				}
			}
			else
			{
				// Agent collided with terrain - store the leftmost landing point
				// TODO: Think this should probably be dealt with by presolve instead on begincontact
				// see note in phys2d_system::PreSolve about an object rotating whilst already contacting.
				// Here we should really pick up such a collision since it may be to the left (min x)
				// of where the initial contact occurred.

				b2WorldManifold manifold;
				contact->GetWorldManifold(&manifold);
				for(size_t p = 0; p < num_points; ++p)
				{
					auto x_pos = manifold.points[p].x;
					if(x_pos > m_landing_x_start &&
						(!m_landing_contact_min_x || x_pos < *m_landing_contact_min_x)
						)
					{
						m_landing_contact_min_x = x_pos;
					}
				}
			}
		}

		bool ski_jump_scenario::is_complete() const
		{
			return false;
		}

		bool ski_jump_scenario::on_platform() const
		{
			for(auto const& val : m_platform_sensors)
			{
				if(val > 0)
				{
					return true;
				}
			}
			return false;
		}

		double ski_jump_scenario::platform_position() const
		{
			// Maybe use average of activated sensor positions, rather than rightmost?
			double pos = 1.0;
			for(auto it = m_platform_sensors.rbegin(); it != m_platform_sensors.rend(); ++it)
			{
				if(*it > 0)
				{
					break;
				}

				pos -= 1.0 / m_platform_sensors.size();
			}

			return pos;
		}

		bool ski_jump_scenario::on_takeoff() const
		{
			for(auto const& val : m_takeoff_sensors)
			{
				if(val > 0)
				{
					return true;
				}
			}
			return false;
		}

		double ski_jump_scenario::takeoff_position() const
		{
			// Maybe use average of activated sensor positions, rather than rightmost?
			double pos = 1.0;
			for(auto it = m_takeoff_sensors.rbegin(); it != m_takeoff_sensors.rend(); ++it)
			{
				if(*it > 0)
				{
					break;
				}

				pos -= 1.0 / m_takeoff_sensors.size();
			}

			return pos;
		}

	}
}


