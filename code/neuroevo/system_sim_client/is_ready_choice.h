// is_ready_choice.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_IS_READY_CHOICE_H
#define __WB_SYSSIM_CLIENT_CONTROL_IS_READY_CHOICE_H

#include "choice_point.h"
#include "inactive_state.h"


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct is_ready_choice: choice_point< is_ready_choice, inactive >
		{
			is_ready_choice(my_context ctx): my_base(ctx) {}

			sc::result react(const make_choice &)
			{
				if(context< system_controller >().is_defn_complete())
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


