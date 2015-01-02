// pole_cart.cpp

#include "pole_cart.h"
#include "pole_cart_defn.h"
#include "phys2d_system.h"
#include "phys2d_entity_data.h"


namespace sys {
	namespace phys2d {

		std::string pole_cart::get_name() const
		{
			return "pole_cart";
		}

		size_t pole_cart::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			auto initial_count = bindings.size();

			// Inherited ones
			composite_body::initialize_state_value_bindings_(bindings, accessors);

			// Biped specific ones
			auto bound_id = accessors.size();
			state_value_id sv_id;

			auto joints = pole_cart_defn::all_joints();
			for(auto jnt : joints)
			{
				sv_id += pole_cart_defn::s_joint_names.at(jnt);

				for(auto jnt_sv : std::set < pole_cart_defn::JointStateValue >{ pole_cart_defn::JointStateValue::Angle, pole_cart_defn::JointStateValue::Speed })
				{
					sv_id += pole_cart_defn::s_joint_sv_names.at(jnt_sv);

					bindings[sv_id] = bound_id++;

					switch(jnt)
					{
						case pole_cart_defn::Joint::Pole:
						switch(jnt_sv)
						{
							case pole_cart_defn::JointStateValue::Angle:
							accessors.push_back([this]
							{
								return pole_joint->GetJointAngle();
							});
							break;
							case pole_cart_defn::JointStateValue::Speed:
							accessors.push_back([this]
							{
								return pole_joint->GetJointSpeed();
							});
							break;
						}
						break;
					}

					sv_id.pop();
				}

				sv_id.pop();
			}

			sv_id = state_value_id::from_string("abs_pole_angle");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return std::abs(pole_joint->GetJointAngle());
			});

			sv_id = state_value_id::from_string("abs_displacement");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return std::abs(std::abs(cart->GetPosition().x));
			});

			sv_id = state_value_id::from_string("failure");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return
					(std::abs(pole_joint->GetJointAngle()) > m_spec->fail_angle ||
					std::abs(std::abs(cart->GetPosition().x)) > m_spec->fail_displacement) ?
					1.0 : 0.0;
			});

			return bindings.size() - initial_count;
		}

		agent_sensor_list pole_cart::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
		{
/*			agent_sensor_list result;
			for(auto const& name : named_inputs)
			{
				result.emplace_back(m_system->m_state_value_ids.at(name));
			}
			return result;
*/
			return{};
		}

		double pole_cart::get_sensor_value(agent_sensor_id const& sensor) const
		{
			return{};// m_system->m_state_value_accessors[sensor]();
		}

		pole_cart::pole_cart(
			std::shared_ptr< pole_cart_defn::spec_data > _spec,
			std::shared_ptr< pole_cart_defn::instance_data > _inst
			)
		{
			m_spec = _spec;
			m_inst = _inst;
		}

		void pole_cart::create(i_system* sys)
		{
// TODO:?			reset_state();
			m_bodies.clear();	// TODO: this should be in composite_body

			auto phys2d_sys = static_cast<phys2d_system /*const*/*>(sys);
			m_sys = phys2d_sys;
			auto world = phys2d_sys->get_world();

			float32 const width = 1.0;
			float32 const height = 0.25;
			float32 const pole_width = 0.1;
			float32 const track_length = m_spec->fail_displacement * 2.0;

			b2BodyDef bd;
			bd.position.Set(0.0f, 0.0f);
			bd.angle = 0.0f;

			bd.type = b2_staticBody;
			track = world->CreateBody(&bd);

			bd.type = b2_dynamicBody;
			cart = world->CreateBody(&bd);

			bd.position.Set(0.0f, m_spec->pole_length / 2);
			pole = world->CreateBody(&bd);

			b2EdgeShape track_shape;
			track_shape.Set(b2Vec2(-track_length / 2.0f, 0.0f), b2Vec2(track_length / 2.0f, 0.0f));

			b2PolygonShape cart_shape;
			cart_shape.SetAsBox(width / 2, height / 2);

			b2PolygonShape pole_shape;
			pole_shape.SetAsBox(pole_width / 2, m_spec->pole_length / 2);


			b2FixtureDef fd;
			fd.filter.groupIndex = -1;

			fd.shape = &track_shape;
			track->CreateFixture(&fd);

			fd.shape = &cart_shape;
			fd.density = 1.0f;
			cart->CreateFixture(&fd);
			b2MassData md;
			md.center.SetZero();
			md.mass = m_spec->cart_mass;
			md.I = 1.0f;	// whatever, can't rotate
			cart->SetMassData(&md);

			fd.shape = &pole_shape;
			fd.density = 1.0f;
			pole->CreateFixture(&fd);
			md.mass = m_spec->pole_mass;
			md.I = m_spec->pole_mass * m_spec->pole_length * m_spec->pole_length / 12.0f;
			pole->SetMassData(&md);

			b2PrismaticJointDef pjd;
			pjd.bodyA = cart;
			pjd.bodyB = track;
			pjd.collideConnected = false;
			pjd.localAxisA = b2Vec2(1.0f, 0.0f);
			pjd.referenceAngle = 0.0f;
			pjd.localAnchorA = b2Vec2(0, 0);
			pjd.localAnchorB = b2Vec2(0, 0);
			pjd.enableLimit = false;
			pjd.enableMotor = false;
			track_joint = (b2PrismaticJoint*)world->CreateJoint(&pjd);

			b2RevoluteJointDef jd;
			jd.enableLimit = true;
			jd.enableMotor = false;
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 0.0f;
			jd.collideConnected = false;

			jd.lowerAngle = -b2_pi / 2;
			jd.upperAngle = b2_pi / 2;

			jd.Initialize(cart, pole, b2Vec2(0.0f, height / 2));
			pole_joint = (b2RevoluteJoint*)world->CreateJoint(&jd);

			add_component_body(track, entity_data::val_t{});
			add_component_body(cart, entity_data::val_t{});
			add_component_body(pole, entity_data::val_t{});

			// TODO: Temp
			auto dist = std::uniform_real_distribution< double >(-m_spec->fail_angle, m_spec->fail_angle);
			double init_angle = dist(phys2d_sys->get_rgen());

			auto pos = pole->GetPosition();
			auto joint_pos = b2Vec2(0.0f, height / 2);	// todo: get from joint?
			pos -= joint_pos;
			pos = b2Vec2((pos.x * std::cos(init_angle)) + (pos.y * -std::sin(init_angle)),
				(pos.x * std::sin(init_angle)) + (pos.y * std::cos(init_angle)));
			pos += joint_pos;
			pole->SetTransform(pos, pole->GetTransform().q.GetAngle() + init_angle);
			//

			// Initial conditions
			phys2d_agent_defn::initialize_object_state(this, *m_inst, sys->get_rgen());
		}

		void pole_cart::activate_effectors(effector_activations const& activations)
		{
			// TODO: Store activations so accessible as state values?

			cart->ApplyForce(b2Vec2(activations[0] * m_spec->max_force, 0.0f), cart->GetPosition(), true);
		}

		void pole_cart::on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type)
		{
			
		}

	}
}



