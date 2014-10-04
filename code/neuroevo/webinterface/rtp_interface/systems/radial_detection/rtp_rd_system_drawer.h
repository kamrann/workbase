// rtp_rd_system_drawer.h

#ifndef __NE_RTP_RD_SYSTEM_DRAWER_H
#define __NE_RTP_RD_SYSTEM_DRAWER_H

#include "../rtp_system_drawer.h"
#include "rtp_radial_detection_system.h"


namespace Wt {
	class WRectF;
}

namespace rtp {

	class rd_system_drawer:
		public static_system_drawer
	{
	public:
		rd_system_drawer(rd_system const& sys);

	public:
		virtual void draw_system(Wt::WPainter& painter, options_t const& options) override;

	private:
		rd_system const& m_sys;
	};

}


#endif

