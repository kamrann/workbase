// phys2d_system_drawer.h

#ifndef __NE_PHYS2D_SYSTEM_DRAWER_H
#define __NE_PHYS2D_SYSTEM_DRAWER_H

#include "system_sim/system_drawer.h"


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class phys2d_system_drawer:
			public static_system_drawer
		{
		public:
			phys2d_system_drawer(phys2d_system const& sys);

		public:
			virtual void draw_system(Wt::WPainter& painter, options_t const& options) override;

		private:
			phys2d_system const& m_sys;
		};

	}
}


#endif

