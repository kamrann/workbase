// skier.cpp
/*
BIPED LEARNING NOTES

Muscle energy: per-muscle value that diminishes proprtionally (not necessarily linearly) with force applied.
Gradually builds back to a maximum when force is below some threshold (obviously rebuilding more quickly the
closer the applied force is to zero).

The energy value affects the force that can be applied at the muscle. Either modulate the controller output
by a function of the current muscle energy, or alternatively modulate the output by a static max force, but then
clamp it to a function of the muscle energy.
*/

#include "skier.h"
#include "skier_defn.h"
#include "phys2d_system.h"
#include "phys2d_entity_data.h"


namespace sys {
	namespace phys2d {

		std::string skier::get_name() const
		{
			return "skier";	// Instance name?
		}

		size_t skier::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			auto initial_count = bindings.size();

			// Inherited ones
			composite_body::initialize_state_value_bindings_(bindings, accessors);

			auto bound_id = accessors.size();
			state_value_id sv_id;

			sv_id = state_value_id::from_string("orientation");
			bindings[sv_id] = bound_id++;
			accessors.push_back([this]
			{
				return torso->GetTransform().q.GetAngle();
			});

			ski_front_sensor->initialize_state_value_bindings_(bindings, accessors,
				state_value_id::from_string("ski_front"));
			ski_back_sensor->initialize_state_value_bindings_(bindings, accessors,
				state_value_id::from_string("ski_back"));

			return bindings.size() - initial_count;
		}

		agent_sensor_list skier::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
		{
			return{};
		}

		double skier::get_sensor_value(agent_sensor_id const& sensor) const
		{
			return{};
		}

		skier::skier(
			std::shared_ptr< skier_defn::spec_data > _spec,
			std::shared_ptr< skier_defn::instance_data > _inst)
		{
			m_spec = _spec;
			m_inst = _inst;
		}

		void skier::create(i_system* sys)
		{
			// TODO: this should be in composite_body
			m_bodies.clear();
			m_revolutes.clear();
			//

			auto phys2d_sys = static_cast<phys2d_system /*const*/ *>(sys);
			m_sys = phys2d_sys;
			auto world = phys2d_sys->get_world();

			auto const SkiBevelLength = m_spec->ski_thickness;
			auto const y_base = 0.0;
			int const GroupIndex = -1;	// TODO: unique value for every object created

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.angle = 0.0f;

			auto const ski_y = y_base;
			bd.position.Set(0.0f, ski_y);
			ski_and_boot = world->CreateBody(&bd);

			b2PolygonShape ski_underside_shape;
			std::vector< b2Vec2 > ski_verts;
			auto const under_proportion = 1.0;// 0.8;
			ski_verts.emplace_back(-m_spec->ski_length / 2 - SkiBevelLength * under_proportion, SkiBevelLength * under_proportion);
			ski_verts.emplace_back(-m_spec->ski_length / 2, 0.0);
			ski_verts.emplace_back(m_spec->ski_length / 2, 0.0);
			ski_verts.emplace_back(m_spec->ski_length / 2 + SkiBevelLength * under_proportion, SkiBevelLength * under_proportion);
			ski_underside_shape.Set(ski_verts.data(), ski_verts.size());

			/* TODO: issue. probably too thin a fixture like this
			b2PolygonShape ski_upper_shape;
			ski_verts[1] = ski_verts[3];
			ski_verts[2] = ski_verts[1] + b2Vec2(SkiBevelLength * (1.0 - under_proportion), SkiBevelLength * (1.0 - under_proportion));
			ski_verts[3] = ski_verts[0] + b2Vec2(-SkiBevelLength * (1.0 - under_proportion), SkiBevelLength * (1.0 - under_proportion));
			ski_upper_shape.Set(ski_verts.data(), ski_verts.size());
			*/

			b2PolygonShape boot_shape;
			boot_shape.SetAsBox(m_spec->boot_width / 2, m_spec->boot_height / 2,
				b2Vec2(0.0, SkiBevelLength + m_spec->boot_height / 2), 0.0);

			b2FixtureDef fd;
			fd.density = 1.0f;
			fd.filter.groupIndex = GroupIndex;

			fd.shape = &ski_underside_shape;
			fd.friction = m_spec->ski_friction;
			fd.restitution = 0.15f;
			ski_and_boot->CreateFixture(&fd);

/*			fd.shape = &ski_upper_shape;
			fd.friction = 0.8f;
			fd.restitution = 0.0;
			ski_and_boot->CreateFixture(&fd);
*/
			fd.friction = 0.5f;
			fd.restitution = 0.0f;

			fd.shape = &boot_shape;
			ski_and_boot->CreateFixture(&fd);


			b2CircleShape ski_back_sensor_shape;
			ski_back_sensor_shape.m_p.Set(-m_spec->ski_length / 2, 0.0);
			ski_back_sensor_shape.m_radius = SkiBevelLength / 2;

			b2CircleShape ski_front_sensor_shape;
			ski_front_sensor_shape.m_p.Set(m_spec->ski_length / 2, 0.0);
			ski_front_sensor_shape.m_radius = SkiBevelLength / 2;

			collision_filter sensor_filter;
			sensor_filter.group_index = GroupIndex;
			ski_back_sensor = contact_sensor_cmp::create(ski_and_boot, &ski_back_sensor_shape, sensor_filter);
			ski_front_sensor = contact_sensor_cmp::create(ski_and_boot, &ski_front_sensor_shape, sensor_filter);


			auto const lower_leg_y = ski_y + SkiBevelLength + m_spec->boot_height;

			auto const LowerLegAngle = -b2_pi / 6;
			auto const lower_leg_x = 0.0;
//			bd.position.Set(0.0f, lower_leg_y + m_spec->lower_leg_length / 2);
//			lower_leg = world->CreateBody(&bd);

			b2PolygonShape lower_leg_shape;
			lower_leg_shape.SetAsBox(
				m_spec->lower_leg_thickness / 2,
				m_spec->lower_leg_length / 2,
				b2Vec2(
				lower_leg_x + m_spec->lower_leg_length * 0.5 * -std::sin(LowerLegAngle),
				lower_leg_y + m_spec->lower_leg_length * 0.5 * std::cos(LowerLegAngle)
				),
				LowerLegAngle
				);

			fd.shape = &lower_leg_shape;
//			lower_leg->CreateFixture(&fd);
			ski_and_boot->CreateFixture(&fd);


			auto const upper_leg_y = lower_leg_y + m_spec->lower_leg_length * std::cos(LowerLegAngle);
			auto const upper_leg_x = lower_leg_x + m_spec->lower_leg_length * -std::sin(LowerLegAngle);
			bd.position.Set(upper_leg_x, upper_leg_y + m_spec->upper_leg_length / 2);
			upper_leg = world->CreateBody(&bd);

			b2PolygonShape upper_leg_shape;
			upper_leg_shape.SetAsBox(m_spec->upper_leg_thickness / 2, m_spec->upper_leg_length / 2);

			fd.shape = &upper_leg_shape;
			upper_leg->CreateFixture(&fd);


			auto const torso_y = upper_leg_y + m_spec->upper_leg_length;
			auto const torso_x = upper_leg_x;
			bd.position.Set(torso_x, torso_y + m_spec->torso_height / 2);
			torso = world->CreateBody(&bd);

			b2PolygonShape torso_shape;
			torso_shape.SetAsBox(m_spec->torso_width / 2, m_spec->torso_height / 2);

			fd.shape = &torso_shape;
			torso->CreateFixture(&fd);
			
			
			auto const shoulder_y_prop = 0.75;	// proportion of the way up the torso
			auto const shoulder_y = torso_y + m_spec->torso_height * shoulder_y_prop;
			auto const shoulder_x = torso_x;
			bd.position.Set(shoulder_x, shoulder_y - m_spec->arm_length / 2);
			arm = world->CreateBody(&bd);

			b2PolygonShape arm_shape;
			arm_shape.SetAsBox(m_spec->arm_thickness / 2, m_spec->arm_length / 2);

			fd.shape = &arm_shape;
			arm->CreateFixture(&fd);

			
			auto const arm_y = shoulder_y - m_spec->arm_length;
			auto const arm_x = shoulder_x;
			bd.angle = -b2_pi / 2;
			bd.position.Set(arm_x - m_spec->pole_length / 2, arm_y);
			pole = world->CreateBody(&bd);

			b2PolygonShape pole_shape;
			pole_shape.SetAsBox(m_spec->pole_thickness / 2, m_spec->pole_length / 2);

			fd.shape = &pole_shape;
			fd.friction = m_spec->pole_friction;
			pole->CreateFixture(&fd);


			add_component_body(ski_and_boot, entity_data::val_t{});
//			add_component_body(lower_leg, entity_data::val_t{});
			add_component_body(upper_leg, entity_data::val_t{});
			add_component_body(torso, entity_data::val_t{});
			add_component_body(arm, entity_data::val_t{});
			add_component_body(pole, entity_data::val_t{});

/*			auto ankle = revolute_joint::create_torque_activation(
				ski_and_boot,
				lower_leg,
				b2Vec2(0.0f, lower_leg_y),
				m_spec->max_ankle_torque,
				std::make_pair< double, double >(-b2_pi / 4, b2_pi / 4),
				false
				);
			add_revolute("ankle", ankle);
*/
			auto knee = revolute_joint::create_speed_activation(//create_torque_activation(
				ski_and_boot,//lower_leg,
				upper_leg,
				b2Vec2(upper_leg_x, upper_leg_y),
				m_spec->max_knee_torque,
				2 * b2_pi,
				std::make_pair< double, double >(0.0, 3 * b2_pi / 4),
				false
				);
			add_revolute("knee", knee);

			auto hip = revolute_joint::create_speed_activation(//create_torque_activation(
				upper_leg,
				torso,
				b2Vec2(torso_x, torso_y),
				m_spec->max_hip_torque,
				1 * b2_pi,
				std::make_pair< double, double >(-3 * b2_pi / 4, 0.0),
				false
				);
			add_revolute("hip", hip);

			auto shoulder = revolute_joint::create_speed_activation(//create_torque_activation(
				torso,
				arm,
				b2Vec2(shoulder_x, shoulder_y),
				m_spec->max_shoulder_torque,
				2 * b2_pi,
				std::make_pair< double, double >(-b2_pi / 9, b2_pi),
				false
				);
			add_revolute("shoulder", shoulder);

			auto wrist = revolute_joint::create_speed_activation(//create_torque_activation(
				arm,
				pole,
				b2Vec2(arm_x, arm_y),
				m_spec->max_wrist_torque,
				2 * b2_pi,
				std::make_pair< double, double >(-b2_pi / 3, b2_pi / 6),
				false
				);
			add_revolute("wrist", wrist);

			// Initial conditions
			phys2d_agent_defn::initialize_object_state(this, *m_inst, sys->get_rgen());
		}

		void skier::activate_effectors(effector_activations const& activations)
		{
			composite_body::activate_effectors(activations);
		}

	}
}



