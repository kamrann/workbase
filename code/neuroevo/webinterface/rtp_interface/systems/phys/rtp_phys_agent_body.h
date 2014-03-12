// rtp_phys_agent_body.h

#ifndef __NE_RTP_PHYS_AGENT_BODY_H
#define __NE_RTP_PHYS_AGENT_BODY_H

#include "../../params/enum_par.h"

//
#include <Wt/WPainter>
//

#include <Box2D/Box2D.h>

#include <string>


namespace rtp_phys {

	class agent_body
	{
	public:
		virtual b2Vec2 get_position() = 0;
		virtual float get_orientation() = 0;

		// Temp, don't really want WPainter in the interface
		virtual void draw(Wt::WPainter& painter) = 0;
	};

}


#endif

