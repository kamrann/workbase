// is_completed_choice.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_IS_COMPLETED_CHOICE_H
#define __WB_SYSSIM_CLIENT_CONTROL_IS_COMPLETED_CHOICE_H

#include "choice_point.h"
#include "notrunning_state.h"


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;

		struct is_completed_choice: choice_point< is_completed_choice, not_running >
		{
			is_completed_choice(my_context ctx): my_base(ctx) {}

			sc::result react(const make_choice &)
			{
				if(context< active >().complete)
				{
					return transit< completed >();
				}
				else
				{
					return transit< paused >();
				}
			}
		};

	}
}


#endif


