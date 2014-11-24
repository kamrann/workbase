// completed_state.cpp

#include "completed_state.h"
#include "control_fsm.h"


namespace sys_control {
	namespace fsm {

		completed::completed(my_context ctx):
			base(ctx)
		{
			auto& ctrl = context< system_controller >();
			ctrl.output_sink("system completed");

			// Output result values
			auto& atv = context< active >();
			if(!atv.result_vals.empty())
			{
				ctrl.output_sink("result state values:");
				atv.output_state_values(atv.result_vals);
			}
		}

		std::string completed::get_prompt() const
		{
			return "@completed>";
		}

	}
}



