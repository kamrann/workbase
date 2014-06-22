// rtp_phys_agent_body.h

#ifndef __NE_RTP_PHYS_AGENT_BODY_H
#define __NE_RTP_PHYS_AGENT_BODY_H

#include "../../params/enum_par.h"

#include "../rtp_agent.h"

//
namespace Wt {
	class WPainter;
}
//

struct b2Vec2;

namespace rtp_phys {

	class agent_body
	{
	public:
		static agent_sensor_list sensor_inputs()
		{
			return{
				"Pos[x]",
				"Pos[y]",
				"LinVel[x]",
				"LinVel[y]",
				"KE",
			};
		}

	public:
		virtual void translate(b2Vec2 const& vec) = 0;
		virtual void rotate(float angle) = 0;
		virtual void set_linear_velocity(b2Vec2 const& vel) = 0;
		virtual void set_angular_velocity(float vel) = 0;

		virtual b2Vec2 get_position() const = 0;
		virtual b2Vec2 get_linear_velocity() const = 0;
		//virtual float get_angular_velocity() const = 0;
		virtual float get_kinetic_energy() const = 0;

		// Temp, don't really want WPainter in the interface
		virtual void draw(Wt::WPainter& painter) const = 0;
	};

}


#endif

