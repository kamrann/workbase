// composite_body.cpp

#include "composite_body.h"
#include "phys2d_entity_data.h"

#include "b2d_util.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		void composite_body::translate(b2Vec2 const& vec)
		{
			for(b2Body* b : m_bodies)
			{
				b->SetTransform(b->GetPosition() + vec, b->GetTransform().q.GetAngle());
			}
		}

		void composite_body::rotate(float angle)
		{
			// Rotate about center of mass
			b2Vec2 com = get_com_position();
			for(b2Body* b : m_bodies)
			{
				// TODO: This definitely correct method???
				b2Vec2 pos = b->GetPosition();
				pos -= com;
				pos = b2Vec2((pos.x * std::cos(angle)) + (pos.y * -std::sin(angle)),
					(pos.x * std::sin(angle)) + (pos.y * std::cos(angle)));
				pos += com;
				b->SetTransform(pos, b->GetTransform().q.GetAngle() + angle);
			}
		}

		void composite_body::set_linear_velocity(b2Vec2 const& linvel)
		{
			for(b2Body* b : m_bodies)
			{
				b->SetLinearVelocity(linvel);
			}
		}

		void composite_body::set_angular_velocity(float angvel)
		{
			b2Vec2 com = get_com_position();
			b2Vec2 v_com = get_com_linear_velocity();
			for(b2Body* b : m_bodies)
			{
				b->SetAngularVelocity(angvel);
				b2Vec2 r = b->GetPosition() - com;
				b->SetLinearVelocity(v_com + angvel * b2Vec2(-r.y, r.x));
			}
		}

		b2Vec2 composite_body::get_position() const
		{
			return get_com_position();
		}

		b2Vec2 composite_body::get_linear_velocity() const
		{
			return get_com_linear_velocity();
		}

		float composite_body::get_kinetic_energy() const
		{
			float ke = 0.0f;
			for(b2Body const* b : m_bodies)
			{
				ke += ::get_kinetic_energy(b);
			}
			return ke;
		}

		b2Vec2 composite_body::get_com_position() const
		{
			b2Vec2 com(0.0f, 0.0f);
			float sum_mass = 0.0f;
			for(b2Body const* b : m_bodies)
			{
				float mass = b->GetMass();
				com += mass * b->GetPosition();
				sum_mass += mass;
			}
			return (1.0f / sum_mass) * com;
		}

		b2Vec2 composite_body::get_com_linear_velocity() const
		{
			b2Vec2 com_vel(0.0f, 0.0f);
			float sum_mass = 0.0f;
			for(b2Body const* b : m_bodies)
			{
				float mass = b->GetMass();
				com_vel += mass * b->GetLinearVelocity();
				sum_mass += mass;
			}
			return (1.0f / sum_mass) * com_vel;
		}

		state_value_id_list composite_body::get_state_values()
		{
			// todo: composite specific
			return object::get_state_values();
		}

		size_t composite_body::initialize_state_value_bindings_(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			auto initial_count = bindings.size();
			auto bound_id = accessors.size();

			bindings[s_sv_names.left.at(StateValue::PositionX)] = bound_id++;
			accessors.push_back([this]
			{
				return get_com_position().x;
			});
			bindings[s_sv_names.left.at(StateValue::PositionY)] = bound_id++;
			accessors.push_back([this]
			{
				return get_com_position().y;
			});
			bindings[s_sv_names.left.at(StateValue::LinVelX)] = bound_id++;
			accessors.push_back([this]
			{
				return get_linear_velocity().x;
			});
			bindings[s_sv_names.left.at(StateValue::LinVelY)] = bound_id++;
			accessors.push_back([this]
			{
				return get_linear_velocity().y;
			});
			bindings[s_sv_names.left.at(StateValue::KE)] = bound_id++;
			accessors.push_back([this]
			{
				return get_kinetic_energy();
			});

			return bindings.size() - initial_count;
		}

		/*
		double composite_body::get_sensor_value(agent_sensor_id const& sensor) const
		{
		switch(sensor)
		{
		default:
		return agent_body::get_sensor_value(sensor);
		}
		}

		void composite_body::draw(Wt::WPainter& painter) const
		{
		for(b2Body const* b : m_bodies)
		{
		::draw_body(b, painter);
		}
		}
		*/
		void composite_body::add_component_body(b2Body* comp, boost::any&& data)
		{
			m_bodies.insert(
				std::end(m_bodies),
				comp);
			entity_data ed{};
			ed.type = entity_data::Type::Agent;
			ed.type_value = static_cast<object*>(this);
			ed.value = std::move(data);
			set_body_data(comp, std::move(ed));
		}

	}
}


