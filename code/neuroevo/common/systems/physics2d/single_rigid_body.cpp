// single_rigid_body.cpp

#include "single_rigid_body.h"
#include "phys2d_entity_data.h"

#include "b2d_util.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		void single_rigid_body::translate(b2Vec2 const& vec)
		{
			m_body->SetTransform(m_body->GetPosition() + vec, m_body->GetTransform().q.GetAngle());
		}

		void single_rigid_body::rotate(float angle)
		{
			// TODO: This is rotating around local origin, which may not be center of mass.
			// Composite body rotates around center of mass. This discrepancy okay?
			m_body->SetTransform(m_body->GetTransform().p, m_body->GetTransform().q.GetAngle() + angle);
		}

		void single_rigid_body::set_linear_velocity(b2Vec2 const& linvel)
		{
			m_body->SetLinearVelocity(linvel);
		}

		void single_rigid_body::set_angular_velocity(float angvel)
		{
			m_body->SetAngularVelocity(angvel);
		}

		void single_rigid_body::set_position(b2Vec2 const& pos)
		{
			m_body->SetTransform(pos, m_body->GetTransform().q.GetAngle());
		}

		void single_rigid_body::set_orientation(float angle)
		{
			m_body->SetTransform(m_body->GetTransform().p, angle);
		}

		void single_rigid_body::apply_force(b2Vec2 const& force, b2Vec2 const& pos)
		{
			m_body->ApplyForce(force, pos, true);
		}

		void single_rigid_body::apply_force_local(b2Vec2 const& force, b2Vec2 const& pos)
		{
			m_body->ApplyForce(
				m_body->GetWorldVector(force),
				m_body->GetWorldPoint(pos),
				true);
		}

		b2Vec2 single_rigid_body::get_position() const
		{
			return m_body->GetPosition();
		}

		float single_rigid_body::get_orientation() const
		{
			return m_body->GetTransform().q.GetAngle();
		}

		b2Vec2 single_rigid_body::get_linear_velocity() const
		{
			return m_body->GetLinearVelocity();
		}

		b2Vec2 single_rigid_body::get_local_linear_velocity() const
		{
			return m_body->GetLocalVector(m_body->GetLinearVelocity());
		}

		float single_rigid_body::get_angular_velocity() const
		{
			return m_body->GetAngularVelocity();
		}

		float single_rigid_body::get_orientation_wrt_linear_velocity() const
		{
			b2Vec2 const linvel = m_body->GetLinearVelocity();
			float vel_angle = std::atan2(linvel.x, linvel.y);
			float angle_diff = m_body->GetTransform().q.GetAngle() - vel_angle;
			if(angle_diff < -b2_pi)
			{
				angle_diff += 2 * b2_pi;
			}
			else if(angle_diff > b2_pi)
			{
				angle_diff -= 2 * b2_pi;
			}
			return angle_diff;
		}

		float single_rigid_body::get_kinetic_energy() const
		{
			return ::get_kinetic_energy(m_body);
		}
		/*
			float single_rigid_body::get_grav_potential_energy() const
			{
			return ::get_grav_potential_energy(m_body, m_body->GetWorld()->GetGravity().y);
			}
			*/
		/*
		double single_rigid_body::get_sensor_value(agent_sensor_id const& sensor) const
		{
			switch(sensor)
			{
				case Orientation:
				return get_orientation();
				case AngularVelocity:
				return get_angular_velocity();
				case LocalVelocity_X:
				return get_local_linear_velocity().x;
				case LocalVelocity_Y:
				return get_local_linear_velocity().y;
				default:
				return agent_body::get_sensor_value(sensor);
			}
		}

		void single_rigid_body::draw(Wt::WPainter& painter) const
		{
			::draw_body(m_body, painter);
		}
		*/
		void single_rigid_body::set_body(b2Body* body)
		{
			m_body = body;
			entity_data ed{};
			ed.type = entity_data::Type::Agent;
			//ed.type_value = static_cast<??*>(this);
			//ed.value = 
			set_body_data(body, std::move(ed));
		}

	}
}

