// composite_rigid_body.h

#ifndef __NE_RTP_PHYS_COMPOSITE_RIGID_BODY_H
#define __NE_RTP_PHYS_COMPOSITE_RIGID_BODY_H

#include "../rtp_phys_agent_body.h"

#include <vector>


class b2Body;

namespace rtp_phys {

	class composite_rigid_body: public agent_body
	{
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

		virtual void draw(Wt::WPainter& painter) const;

	protected:
		std::vector< b2Body* > m_bodies;
	};

}


#endif

