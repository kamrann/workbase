// composite_rigid_body.h

#ifndef __NE_RTP_PHYS_COMPOSITE_RIGID_BODY_H
#define __NE_RTP_PHYS_COMPOSITE_RIGID_BODY_H

#include "../rtp_phys_agent_body.h"

#include <boost/any.hpp>

#include <list>


class b2Body;

namespace rtp {

	class composite_rigid_body: public agent_body
	{
	public:
		typedef agent_body base_t;

		enum Sensors {
			_Prev = base_t::Sensors::_Next - 1,

			_Next,
		};

		static agent_sensor_name_list sensor_inputs()
		{
			auto inputs = base_t::sensor_inputs();
/*			inputs.insert(
				end(inputs),
				{
					std::string("Orientation"),
					std::string("AngVel"),
				});
*/			return inputs;
		}

		using agent_body::agent_body;

	public:
		virtual void translate(b2Vec2 const& vec);
		virtual void rotate(float angle);
		virtual void set_linear_velocity(b2Vec2 const& vel);
		virtual void set_angular_velocity(float vel);

		virtual b2Vec2 get_position() const;
		virtual b2Vec2 get_linear_velocity() const;
		virtual float get_kinetic_energy() const;

		virtual b2Vec2 get_com_position() const;
		virtual b2Vec2 get_com_linear_velocity() const;

		virtual double get_sensor_value(agent_sensor_id const& sensor) const;

		virtual void draw(Wt::WPainter& painter) const;

	protected:
		void add_component_body(b2Body* comp, boost::any&& data);

	protected:
		std::list< b2Body* > m_bodies;
	};

}


#endif

