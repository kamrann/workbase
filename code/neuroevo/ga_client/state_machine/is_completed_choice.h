// is_completed_choice.h

#ifndef __WB_GA_CLIENT_CONTROL_IS_COMPLETED_CHOICE_H
#define __WB_GA_CLIENT_CONTROL_IS_COMPLETED_CHOICE_H

#include "clsm/choice_point.h"
#include "notrunning_state.h"


namespace ga_control {
	namespace fsm {

		namespace sc = boost::statechart;

		struct paused;
		struct completed;

		struct is_completed_choice: sc::choice_point< is_completed_choice, not_running >
		{
			is_completed_choice(my_context ctx): my_base(ctx) {}

			sc::result react(const sc::make_choice &)
			{
				if(false)//context< active >().complete)
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


