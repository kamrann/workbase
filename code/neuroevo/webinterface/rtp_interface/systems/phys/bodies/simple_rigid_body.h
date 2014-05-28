// simple_rigid_body.h

#ifndef __NE_RTP_PHYS_SIMPLE_RIGID_BODY_H
#define __NE_RTP_PHYS_SIMPLE_RIGID_BODY_H

#include "../rtp_phys_agent_body.h"


class b2Body;

namespace rtp_phys {

	class simple_rigid_body: public agent_body
	{
	public:
		virtual void translate(b2Vec2 const& vec);
		virtual void rotate(float angle);
		virtual void set_linear_velocity(b2Vec2 const& linvel);
		virtual void set_angular_velocity(float angvel);

		virtual void set_position(b2Vec2 const& pos);
		virtual void set_orientation(float angle);
		virtual void apply_force(b2Vec2 const& force, b2Vec2 const& pos);
		virtual void apply_force_local(b2Vec2 const& lforce, b2Vec2 const& lpos);

		virtual b2Vec2 get_position() const;
		virtual b2Vec2 get_linear_velocity() const;
		virtual float get_kinetic_energy() const;

		virtual float get_orientation() const;
		virtual float get_angular_velocity() const;
		virtual float get_orientation_wrt_linear_velocity() const;

		virtual void draw(Wt::WPainter& painter) const;

	public:
		b2Body* m_body;
	};

}


#endif

