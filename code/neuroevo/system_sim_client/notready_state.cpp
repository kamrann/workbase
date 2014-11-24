// notready_state.cpp

#include "notready_state.h"
#include "control_fsm.h"


namespace sys_control {
	namespace fsm {

		std::string not_ready::get_prompt() const
		{
			return "@not ready>";
		}

	}
}




