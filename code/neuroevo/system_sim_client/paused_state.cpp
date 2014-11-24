// paused_state.cpp

#include "paused_state.h"
#include "control_fsm.h"


namespace sys_control {
	namespace fsm {

		struct running;

		sc::result paused::react(clsm::ev_cmd< step_cmd > const& cmd)
		{
			// Step system
			auto frames = cmd.frames ? *cmd.frames : 1;
			auto still_going = context< active >().step_system(frames);

			if(still_going)
			{
				// No transition
				return discard_event();
			}
			else
			{
				// Transition to completed state
				return transit< completed >();
			}
		}

		sc::result paused::react(clsm::ev_cmd< run_cmd > const& cmd)
		{
			// need to pass ev.frame_rate to initialization of running state...
			post_event(cmd);
			return transit< running >();
		}

		std::string paused::get_prompt() const
		{
			return "@paused>";
		}

	}
}




