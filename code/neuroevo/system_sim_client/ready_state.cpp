// ready_state.cpp

#include "ready_state.h"
#include "control_fsm.h"


namespace sys_control {
	namespace fsm {

		std::string ready::get_prompt() const
		{
			return "@ready>";
		}

	}
}



