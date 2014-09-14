// rtp_sat_system_drawer.h

#ifndef __NE_RTP_SAT_SYSTEM_DRAWER_H
#define __NE_RTP_SAT_SYSTEM_DRAWER_H

#include "../rtp_system_drawer.h"
#include "../rtp_system_accessor.h"

#include "util/dimensionality.h"


namespace rtp_sat {

	template < WorldDimensionality dim >
	class sat_system;

	template < WorldDimensionality dim >
	class sat_system_drawer:
		public i_system_drawer
//		, public async_system_drawer
	{
	public:
		sat_system_drawer(sat_system< dim > const& sys);

	public:
		virtual void draw_system(Wt::WPainter& painter);

	private:
		sat_system< dim > const& m_sys;
	};

}


#endif

