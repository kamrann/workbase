// paused_state.cpp

#include "paused_state.h"
#include "control_fsm.h"


namespace ga_control {
	namespace fsm {

		struct running;

		paused::paused(my_context ctx): base(ctx)
		{
			reg_cmd< step_cmd >(wrap_grammar< step_cmd_parser< clsm::iter_t > >());
			reg_cmd< run_cmd >(wrap_grammar< run_cmd_parser< clsm::iter_t > >());
			reg_cmd< chart_cmd >(wrap_grammar< chart_cmd_parser< clsm::iter_t > >());
		}

		sc::result paused::react(clsm::ev_cmd< step_cmd > const& cmd)
		{
			// Step system
			auto generations = cmd.generations ? *cmd.generations : 1;
			context< active >().do_generations(generations);

			return transit< not_running >();
/*			context< ga_controller >().serializer->serialize_pause();
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
*/		}

		sc::result paused::react(clsm::ev_cmd< run_cmd > const& cmd)
		{
			// need to pass ev.frame_rate to initialization of running state...
			post_event(cmd);
			return transit< running >();
		}

		std::string paused::get_prompt() const
		{
			std::stringstream ss;
			ss << "@paused" << "[" << context< active >().alg->generation() << "]" << ">";
			return ss.str();
		}

	}
}




