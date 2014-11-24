// is_ready_choice.h

#ifndef __WB_GA_CLIENT_CONTROL_IS_READY_CHOICE_H
#define __WB_GA_CLIENT_CONTROL_IS_READY_CHOICE_H

#include "clsm/choice_point.h"
#include "inactive_state.h"


namespace ga_control {
	namespace fsm {

		namespace sc = boost::statechart;

		struct ready;
		struct not_ready;

		struct is_ready_choice: sc::choice_point< is_ready_choice, inactive >
		{
			is_ready_choice(my_context ctx): my_base(ctx) {}

			sc::result react(const sc::make_choice &)
			{
				if(context< ga_controller >().is_defn_complete())
				{
					return transit< ready >();
				}
				else
				{
					return transit< not_ready >();
				}
			}
		};

	}
}


#endif


