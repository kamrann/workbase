// editing_state.cpp

#include "editing_state.h"
#include "control_fsm.h"


namespace ga_control {
	namespace fsm {

		editing::editing(my_context ctx):
			base(ctx)
			, editor(
			std::move(context< ga_controller >().ptree),
			context< ga_controller >().provider,
			context< ga_controller >().output_sink,
			"exit"
			)
		{
			editor.initiate();
		}

		sc::result editing::react(clsm::ev_cmd_str const& cmd)
		{
			editor.process_event(cmd);
			if(editor.terminated())
			{
				context< ga_controller >().ptree = std::move(editor.ptree);
				return transit< inactive >();
			}
			else
			{
				// TODO: Ideally want to know if editor processed the command, and if not,
				// forward to base class
				//return base::react(cmd);

				return discard_event();
			}
		}

		std::string editing::get_prompt() const
		{
			return editor.get_prompt();
		}

	}
}




