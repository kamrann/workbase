// rtp_phys_system_drawer.h

#ifndef __NE_RTP_PHYS_SYSTEM_DRAWER_H
#define __NE_RTP_PHYS_SYSTEM_DRAWER_H

#include "../rtp_system_drawer.h"
#include "../rtp_system_accessor.h"


namespace rtp {

	class phys_system;

	class phys_system_drawer:
		public static_system_drawer
//		, public async_system_drawer
	{
	public:
		phys_system_drawer(phys_system const& sys);

	public:
		virtual void draw_system(Wt::WPainter& painter, options_t const& options) override;

	private:
		phys_system const& m_sys;
	};

}


#endif

