// snakebot.cpp

#include "snakebot.h"
#include "snakebot_defn.h"
#include "phys2d_system.h"
#include "phys2d_entity_data.h"


namespace sys {
	namespace phys2d {

		std::string snakebot::get_name() const
		{
			return "snakebot";	// Instance name?
		}

		size_t snakebot::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			auto initial_count = bindings.size();

			// Inherited ones
			composite_body::initialize_state_value_bindings_(bindings, accessors);

			auto bound_id = accessors.size();
			state_value_id sv_id;

			for(size_t s = 0; s < m_spec->num_segments; ++s)
			{
				auto prefix = std::string{ "sucker" } +std::to_string(s + 1);
				suckers[s]->initialize_state_value_bindings_(bindings, accessors, prefix);

/*				sv_id = state_value_id::from_string(std::string{ "contact" } + std::to_string(s + 1));
				bindings[sv_id] = bound_id++;
				accessors.push_back([this, s]
				{
					return suckers[s].contact ? 1.0 : 0.0;
				});
*/			}

			return bindings.size() - initial_count;
		}

		agent_sensor_list snakebot::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
		{
			return{};
		}

		double snakebot::get_sensor_value(agent_sensor_id const& sensor) const
		{
			return{};
		}

		snakebot::snakebot(
			std::shared_ptr< snakebot_defn::spec_data > _spec,
			std::shared_ptr< snakebot_defn::instance_data > _inst)
		{
			m_spec = _spec;
			m_inst = _inst;
		}

		void snakebot::create(i_system* sys)
		{
			// TODO: this should be in composite_body
			m_bodies.clear();
			m_revolutes.clear();
			//
			segments.clear();
			suckers.clear();

			auto phys2d_sys = static_cast<phys2d_system /*const*/ *>(sys);
			m_sys = phys2d_sys;
			world = phys2d_sys->get_world();

			int const GroupIndex = -1;	// TODO:
				
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.angle = 0.0f;

			collision_filter sensor_filter;
			sensor_filter.group_index = GroupIndex;
			for(size_t i = 0; i < m_spec->num_segments; ++i)
			{
				bd.position.Set(m_spec->segment_size * i, m_spec->segment_size / 2);
				auto seg = world->CreateBody(&bd);

				b2PolygonShape seg_shape;
				seg_shape.SetAsBox(m_spec->segment_size / 2, m_spec->segment_size / 2);

				b2FixtureDef fd;
				fd.density = 1.0f;
				fd.friction = m_spec->friction;
				fd.filter.groupIndex = GroupIndex;

				fd.shape = &seg_shape;
				seg->CreateFixture(&fd);

				segments.push_back(seg);

				suckers.push_back(sucker_cmp::create(
					seg,
					b2Vec2(0.0, -m_spec->segment_size / 2),
					m_spec->segment_size / 4,
					sensor_filter
					));
			}

			for(auto s : segments)
			{
				add_component_body(s, entity_data::val_t{});
			}

			for(size_t i = 0; i < m_spec->num_segments - 1; ++i)
			{
				auto jnt_name = "joint" + std::to_string(i + 1);

				std::shared_ptr< revolute_joint > rev;
				if(m_spec->tracking == "torque")
				{
					rev = revolute_joint::create_torque_activation(
						segments[i],
						segments[i + 1],
						b2Vec2(m_spec->segment_size * (i + 0.5), m_spec->segment_size / 2),
						m_spec->max_torque,
						std::make_pair(-m_spec->max_angle, m_spec->max_angle),
						false
						);
				}
				else if(m_spec->tracking == "speed")
				{
					rev = revolute_joint::create_speed_activation(
						segments[i],
						segments[i + 1],
						b2Vec2(m_spec->segment_size * (i + 0.5), m_spec->segment_size / 2),
						m_spec->max_torque,
						b2_pi / 2, // todo:
						std::make_pair(-m_spec->max_angle, m_spec->max_angle),
						false
						);
				}
				else if(m_spec->tracking == "position")
				{
					rev = revolute_joint::create_position_activation(
						segments[i],
						segments[i + 1],
						b2Vec2(m_spec->segment_size * (i + 0.5), m_spec->segment_size / 2),
						m_spec->max_torque,
						2.0, // todo:
						std::make_pair(-m_spec->max_angle, m_spec->max_angle),
						false
						);
				}

				add_revolute(//joints[i], m_spec->max_torque, jnt_name);
					jnt_name,
					rev
					);
			}

			// Initial conditions
			phys2d_agent_defn::initialize_object_state(this, *m_inst, sys->get_rgen());
		}

		void snakebot::activate_effectors(effector_activations const& activations)
		{
			auto num_joints = m_spec->num_segments - 1;
			auto joint_activations = effector_activations{
				std::begin(activations),
				std::begin(activations) + num_joints
			};
			composite_body::activate_effectors(joint_activations);

			// Last activations are suckers
			size_t sck_idx = 0;
			for(auto a_it = std::begin(activations) + num_joints;
				a_it != std::begin(activations) + num_joints + m_spec->num_segments;
				++a_it, ++sck_idx)
			{
				suckers[sck_idx]->apply_activation(*a_it);
			}
		}

	}
}



