// basic_biped.cpp
/*
BIPED LEARNING NOTES

Muscle energy: per-muscle value that diminishes proprtionally (not necessarily linearly) with force applied.
Gradually builds back to a maximum when force is below some threshold (obviously rebuilding more quickly the
closer the applied force is to zero).

The energy value affects the force that can be applied at the muscle. Either modulate the controller output
by a function of the current muscle energy, or alternatively modulate the output by a static max force, but then
clamp it to a function of the muscle energy.
*/

#include "basic_biped.h"
#include "basic_biped_defn.h"
#include "phys2d_system.h"
#include "phys2d_entity_data.h"


namespace sys {
	namespace phys2d {

		std::string basic_biped::get_name() const
		{
			return "Basic Biped";	// Instance name?
		}

		size_t basic_biped::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			auto initial_count = bindings.size();

			// Inherited ones
			composite_body::initialize_state_value_bindings_(bindings, accessors);

			// Biped specific ones
			auto bound_id = accessors.size();
			state_value_id sv_id;
#if 0 TODO:
			auto has_upper = false;// TODO:
			auto joint_set = has_upper ? basic_biped_defn::all_joints() : basic_biped_defn::lower_joints();
			for(auto jnt : joint_set)
			{
				sv_id += basic_biped_defn::s_joint_names.at(jnt);

				for(auto jnt_sv_i = 0; jnt_sv_i < (int)basic_biped_defn::JointStateValue::Count; ++jnt_sv_i)
				{
					auto jnt_sv = (basic_biped_defn::JointStateValue)jnt_sv_i;

					sv_id += basic_biped_defn::s_joint_sv_names.at(jnt_sv);

					bindings[sv_id] = bound_id++;

					switch(jnt)
					{
						case basic_biped_defn::Joint::LeftKnee:
						switch(jnt_sv)
						{
							case basic_biped_defn::JointStateValue::Angle:
							accessors.push_back([this]
							{
								return knee1->GetJointAngle();
							});
							break;
							case basic_biped_defn::JointStateValue::Speed:
							accessors.push_back([this]
							{
								return knee1->GetJointSpeed();
							});
							break;
							case basic_biped_defn::JointStateValue::Energy:
							accessors.push_back([this]
							{
								return muscle_states.at(basic_biped_defn::Joint::LeftKnee).energy;
							});
							break;
							case basic_biped_defn::JointStateValue::Torque:
							accessors.push_back([this]
							{
								return muscle_states.at(basic_biped_defn::Joint::LeftKnee).applied_torque;
							});
							break;
						}
						break;

						case basic_biped_defn::Joint::RightKnee:
						switch(jnt_sv)
						{
							case basic_biped_defn::JointStateValue::Angle:
							accessors.push_back([this]
							{
								return knee2->GetJointAngle();
							});
							break;
							case basic_biped_defn::JointStateValue::Speed:
							accessors.push_back([this]
							{
								return knee2->GetJointSpeed();
							});
							break;
							case basic_biped_defn::JointStateValue::Energy:
							accessors.push_back([this]
							{
								return muscle_states.at(basic_biped_defn::Joint::RightKnee).energy;
							});
							break;
							case basic_biped_defn::JointStateValue::Torque:
							accessors.push_back([this]
							{
								return muscle_states.at(basic_biped_defn::Joint::RightKnee).applied_torque;
							});
							break;
						}
						break;

						case basic_biped_defn::Joint::LeftHip:
						switch(jnt_sv)
						{
							case basic_biped_defn::JointStateValue::Angle:
							accessors.push_back([this]
							{
								return hip1->GetJointAngle();
							});
							break;
							case basic_biped_defn::JointStateValue::Speed:
							accessors.push_back([this]
							{
								return hip1->GetJointSpeed();
							});
							break;
							case basic_biped_defn::JointStateValue::Energy:
							accessors.push_back([this]
							{
								return muscle_states.at(basic_biped_defn::Joint::LeftHip).energy;
							});
							break;
							case basic_biped_defn::JointStateValue::Torque:
							accessors.push_back([this]
							{
								return muscle_states.at(basic_biped_defn::Joint::LeftHip).applied_torque;
							});
							break;
						}
						break;

						case basic_biped_defn::Joint::RightHip:
						switch(jnt_sv)
						{
							case basic_biped_defn::JointStateValue::Angle:
							accessors.push_back([this]
							{
								return hip2->GetJointAngle();
							});
							break;
							case basic_biped_defn::JointStateValue::Speed:
							accessors.push_back([this]
							{
								return hip2->GetJointSpeed();
							});
							break;
							case basic_biped_defn::JointStateValue::Energy:
							accessors.push_back([this]
							{
								return muscle_states.at(basic_biped_defn::Joint::RightHip).energy;
							});
							break;
							case basic_biped_defn::JointStateValue::Torque:
							accessors.push_back([this]
							{
								return muscle_states.at(basic_biped_defn::Joint::RightHip).applied_torque;
							});
							break;
						}
						break;
					}

					sv_id.pop();
				}

				sv_id.pop();
			}
#endif

			sv_id = state_value_id::from_string("left_foot_contact");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return left_foot_contact ? 1.0 : 0.0;
			});

			sv_id = state_value_id::from_string("right_foot_contact");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return right_foot_contact ? 1.0 : 0.0;
			});

			sv_id = state_value_id::from_string("both_foot_contact");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return (left_foot_contact && right_foot_contact) ? 1.0 : 0.0;
			});

			return bindings.size() - initial_count;
		}

		agent_sensor_list basic_biped::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
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

		double basic_biped::get_sensor_value(agent_sensor_id const& sensor) const
		{
			return{};// m_system->m_state_value_accessors[sensor]();
		}

		basic_biped::basic_biped(
			std::shared_ptr< basic_biped_defn::spec_data > _spec,
			std::shared_ptr< basic_biped_defn::instance_data > _inst
//			, phys2d_system* system
			)
//			: composite_body(system)
		{
			m_spec = _spec;
			m_inst = _inst;
		}

		void basic_biped::create(i_system* sys)
		{
// TODO:?			reset_state();
			m_bodies.clear();	// TODO: this should be in composite_body

			auto phys2d_sys = static_cast<phys2d_system /*const*/*>(sys);
			m_sys = phys2d_sys;
			auto world = phys2d_sys->get_world();

			float32 const initial_y = m_spec->pelvis_height / 2 + m_spec->upper_len + m_spec->lower_len + m_spec->foot_radius + 0.1f;	// TODO: temp +0.1
			float32 const initial_hip_angle = 0;
			float32 const initial_knee_angle = 0;

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.angle = 0.0f;

			bd.position.Set(0.0f, initial_y);
			pelvis = world->CreateBody(&bd);

			bd.position.Set(0.0f, initial_y - m_spec->pelvis_height / 2 - m_spec->upper_len / 2);
			upper1 = world->CreateBody(&bd);
			upper2 = world->CreateBody(&bd);

			bd.position.Set(0.0f, initial_y - m_spec->pelvis_height / 2 - m_spec->upper_len - m_spec->lower_len / 2);
			lower1 = world->CreateBody(&bd);
			lower2 = world->CreateBody(&bd);

			b2PolygonShape pelvis_shape;
			pelvis_shape.SetAsBox(m_spec->pelvis_width / 2, m_spec->pelvis_height / 2);

			b2PolygonShape upper_shape;
			upper_shape.SetAsBox(m_spec->upper_width / 2, m_spec->upper_len / 2);

			b2PolygonShape lower_shape;
			lower_shape.SetAsBox(m_spec->lower_width / 2, m_spec->lower_len / 2);

			b2CircleShape foot_shape;
			foot_shape.m_p.Set(0.0f, -m_spec->lower_len / 2);
			foot_shape.m_radius = m_spec->foot_radius;

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
#if 0
			b2RevoluteJointDef jd;
			jd.enableLimit = true;
			jd.enableMotor = false;// true;
			jd.motorSpeed = -1.0f;
			jd.maxMotorTorque = 100.0f;
			jd.collideConnected = false;

			// Hip joints
			jd.lowerAngle = -b2_pi / 9;
			jd.upperAngle = b2_pi / 2;

			jd.Initialize(pelvis, upper1, b2Vec2(0.0f, initial_y - m_spec->pelvis_height / 2));
			/*hip1 =*/joints[(int)basic_biped_defn::Joint::LeftHip] = (b2RevoluteJoint*)world->CreateJoint(&jd);

			jd.Initialize(pelvis, upper2, b2Vec2(0.0f, initial_y - m_spec->pelvis_height / 2));
			/*hip2 = */ joints[(int)basic_biped_defn::Joint::RightHip] = (b2RevoluteJoint*)world->CreateJoint(&jd);

			// Knee joints
			jd.lowerAngle = -3 * b2_pi / 4;
			jd.upperAngle = 0;

			jd.Initialize(upper1, lower1, b2Vec2(0.0f, initial_y - m_spec->pelvis_height / 2 - m_spec->upper_len));
			/*knee1 =*/joints[(int)basic_biped_defn::Joint::LeftKnee] = (b2RevoluteJoint*)world->CreateJoint(&jd);

			jd.Initialize(upper2, lower2, b2Vec2(0.0f, initial_y - m_spec->pelvis_height / 2 - m_spec->upper_len));
			/*knee2 =*/joints[(int)basic_biped_defn::Joint::RightKnee] = (b2RevoluteJoint*)world->CreateJoint(&jd);
#endif
			add_component_body(pelvis, entity_data::val_t{});
			add_component_body(upper1, entity_data::val_t{});
			add_component_body(upper2, entity_data::val_t{});
			add_component_body(lower1, entity_data::val_t{});
			add_component_body(lower2, entity_data::val_t{});


			auto left_hip = revolute_joint::create_torque_activation(
				pelvis,
				upper1,
				b2Vec2(0.0f, initial_y - m_spec->pelvis_height / 2),
				m_spec->joint_data.at(basic_biped_defn::Joint::LeftHip).max_torque,
				std::make_pair< double, double >(-b2_pi / 9, b2_pi / 2),
				false
				);
			add_revolute(basic_biped_defn::s_joint_names.at(basic_biped_defn::Joint::LeftHip), left_hip);

			auto right_hip = revolute_joint::create_torque_activation(
				pelvis,
				upper2,
				b2Vec2(0.0f, initial_y - m_spec->pelvis_height / 2),
				m_spec->joint_data.at(basic_biped_defn::Joint::RightHip).max_torque,
				std::make_pair< double, double >(-b2_pi / 9, b2_pi / 2),
				false
				);
			add_revolute(basic_biped_defn::s_joint_names.at(basic_biped_defn::Joint::RightHip), right_hip);

			auto left_knee = revolute_joint::create_torque_activation(
				upper1,
				lower1,
				b2Vec2(0.0f, initial_y - m_spec->pelvis_height / 2 - m_spec->upper_len),
				m_spec->joint_data.at(basic_biped_defn::Joint::LeftKnee).max_torque,
				std::make_pair< double, double >(-3 * b2_pi / 4, 0.0),
				false
				);
			add_revolute(basic_biped_defn::s_joint_names.at(basic_biped_defn::Joint::LeftKnee), left_knee);

			auto right_knee = revolute_joint::create_torque_activation(
				upper2,
				lower2,
				b2Vec2(0.0f, initial_y - m_spec->pelvis_height / 2 - m_spec->upper_len),
				m_spec->joint_data.at(basic_biped_defn::Joint::RightKnee).max_torque,
				std::make_pair< double, double >(-3 * b2_pi / 4, 0.0),
				false
				);
			add_revolute(basic_biped_defn::s_joint_names.at(basic_biped_defn::Joint::RightKnee), right_knee);

			// todo: upper body joints


			// Initial conditions
			phys2d_agent_defn::initialize_object_state(this, *m_inst, sys->get_rgen());

			left_foot_contact = false;
			right_foot_contact = false;

			damage = 0.0;
			life = 1.0;

			for(size_t j = 0; j < (size_t)basic_biped_defn::Joint::Count; ++j)
			{
				auto jnt = (basic_biped_defn::Joint)j;

				muscle_states[jnt].energy = 1.0;
				muscle_states[jnt].applied_torque = 0.0;
			}
		}

/*		void basic_biped::set_system(phys2d_system const* sys)
		{
			m_system = sys;
		}
*/
		double basic_biped::get_force_strength(basic_biped_defn::Joint jnt, double desired)
		{
			auto const maximum = m_spec->joint_data.at(jnt).max_torque;
			auto& energy = muscle_states.at(jnt).energy;

			// for now just linear cap
			auto modulation = std::min(std::abs(desired), energy * life);
			auto strength = std::copysign(modulation * maximum, desired);

			auto const timestep = 1.0 / 30.0;	// TODO: !!!!!!!!!!
			// perhaps affect on energy levels should be dependent on effort in applying force, rather than 
			// force that resulted. ie. original desired value, rather than modulated one.
			auto const power = 2.0; /*??*/
			auto const eqilibrium_force = 0.5;	// the modulation level at which force can be constantly applied without energy change
				// below this and energy will increase, above and it will decrease
			energy -= std::pow(modulation, power) * timestep * 1.0;	// this means will take 1s to exhaust full energy applying max force
			energy += std::pow(eqilibrium_force, power) * timestep;
			energy = std::max(0.0, energy);
			energy = std::min(1.0, energy);

			muscle_states.at(jnt).applied_torque = strength;

			return strength;
		}

		void basic_biped::activate_effectors(effector_activations const& activations)
		{
			// TODO: Store activations so accessible as state values?

			auto left_hip_tq = get_force_strength(basic_biped_defn::Joint::LeftHip, activations[0]);
			pelvis->ApplyTorque(-left_hip_tq, true);
			upper1->ApplyTorque(left_hip_tq, true);
			auto right_hip_tq = get_force_strength(basic_biped_defn::Joint::RightHip, activations[1]);
			pelvis->ApplyTorque(-right_hip_tq, true);
			upper2->ApplyTorque(right_hip_tq, true);
			auto left_knee_tq = get_force_strength(basic_biped_defn::Joint::LeftKnee, activations[2]);
			upper1->ApplyTorque(-left_knee_tq, true);
			lower1->ApplyTorque(left_knee_tq, true);
			auto right_knee_tq = get_force_strength(basic_biped_defn::Joint::RightKnee, activations[3]);
			upper2->ApplyTorque(-right_knee_tq, true);
			lower2->ApplyTorque(right_knee_tq, true);

			// TODO: Upper
		}

		void basic_biped::on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type)
		{
			// fixA is us, fixB could be anything
			auto fd = get_fixture_data(fixA);
			if(fd && !fd->value.empty())
			{
				// TODO:
				auto val = boost::any_cast<int>(fd->value);
				if(val == 0)
				{
					left_foot_contact = (type == ContactType::Begin);
				}
				else if(val == 1)
				{
					right_foot_contact = (type == ContactType::Begin);
				}
			}
			else if(type == ContactType::Begin)
			{
				// Non-foot contact
				damage += 1.0;

				if(m_spec->contact_damage)
				{
					// TODO: For now, insta death
					life = 0.0;
				}
			}
		}

	}
}



