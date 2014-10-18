// phys2d_systemstatevalues.h

#ifndef __NE_PHYS2D_SYSTEM_STATE_VALUES_H
#define __NE_PHYS2D_SYSTEM_STATE_VALUES_H

#include "phys2d_system_defn.h"


namespace sys {
	namespace phys2d {

		enum class phys2d_system_defn::StateValue: unsigned long {
			Time,

			Count,
		};

	}
}


#endif

