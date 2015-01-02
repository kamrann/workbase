// magbot.cpp

#include "magbot.h"
#include "magbot_defn.h"
#include "phys2d_system.h"
#include "phys2d_entity_data.h"


namespace sys {
	namespace phys2d {

		std::string magbot::get_name() const
		{
			return "magbot";	// Instance name?
		}

		size_t magbot::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			auto initial_count = bindings.size();

			// Inherited ones
			composite_body::initialize_state_value_bindings_(bindings, accessors);

			auto bound_id = accessors.size();
			state_value_id sv_id;

			magnet->initialize_state_value_bindings_(bindings, accessors,
				state_value_id::from_string("magnet"));

			return bindings.size() - initial_count;
		}

		agent_sensor_list magbot::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
		{
			return{};
		}

		double magbot::get_sensor_value(agent_sensor_id const& sensor) const
		{
			return{};
		}

		magbot::magbot(
			std::shared_ptr< magbot_defn::spec_data > _spec,
			std::shared_ptr< magbot_defn::instance_data > _inst)
		{
			m_spec = _spec;
			m_inst = _inst;
		}

		void magbot::create(i_system* sys)
		{
			// TODO: this should be in composite_body
			m_bodies.clear();
			m_revolutes.clear();
			//

			auto phys2d_sys = static_cast<phys2d_system /*const*/ *>(sys);
			m_sys = phys2d_sys;
			auto world = phys2d_sys->get_world();

			auto const y_base = 0.0;
			int const GroupIndex = -1;	// TODO: unique value for every object created

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.angle = 0.0f;

			bd.position.Set(0.0f, y_base);
			body = world->CreateBody(&bd);

			b2PolygonShape body_shape;
			body_shape.SetAsBox(m_spec->torso_width / 2, m_spec->torso_height / 2,
				b2Vec2(0.0, m_spec->torso_height / 2), 0.0);

			b2FixtureDef fd;
			fd.density = 1.0f;
			fd.filter.groupIndex = GroupIndex;

			fd.shape = &body_shape;
			fd.friction = 1.0;
			body->CreateFixture(&fd);

			bd.position.Set(0.0f, y_base + m_spec->torso_height);
			arm = world->CreateBody(&bd);

			b2PolygonShape arm_shape;
			arm_shape.SetAsBox(m_spec->arm_thickness / 2, m_spec->arm_length / 2,
				b2Vec2(0.0, m_spec->arm_length / 2), 0.0);

			fd.shape = &arm_shape;
			fd.density = 0.5;
			arm->CreateFixture(&fd);


			add_component_body(body, entity_data::val_t{});
			add_component_body(arm, entity_data::val_t{});

			auto arm_joint = revolute_joint::create_speed_activation(
				body,
				arm,
				b2Vec2(0.0, m_spec->torso_height),
				m_spec->max_arm_torque,
				b2_pi,
				std::make_pair< double, double >(-3 * b2_pi / 4, 3 * b2_pi / 4),
				false
				);
			add_revolute("arm_joint", arm_joint);

			magnet = magnet_cmp::create(
				arm,
				b2Vec2(0.0, m_spec->arm_length),
				m_spec->magnet_strength
				);
			phys2d_sys->register_updatable([this](phys2d_system*)
			{
				this->magnet->update();
			});

			// Initial conditions
			phys2d_agent_defn::initialize_object_state(this, *m_inst, sys->get_rgen());
		}

		void magbot::activate_effectors(effector_activations const& activations)
		{
			auto const num_joints = m_revolutes.size();
			auto joint_activations = effector_activations{ std::begin(activations), std::begin(activations) + num_joints };
			composite_body::activate_effectors(joint_activations);

			magnet->apply_activation(activations[num_joints]);
		}

	}
}



