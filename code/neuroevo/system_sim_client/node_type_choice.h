// node_type_choice.h

#ifndef __WB_PARAMTREE_FSM_NODE_TYPE_CHOICE_H
#define __WB_PARAMTREE_FSM_NODE_TYPE_CHOICE_H

#include "choice_point.h"

#include "base_node_state.h"
#include "general_state.h"
#include "enum_state.h"
#include "random_state.h"
#include "repeat_state.h"


namespace prm {
	namespace fsm {

		struct node_type_choice: sc::choice_point< node_type_choice, base_node_state >
		{
			node_type_choice(my_context ctx): my_base(ctx) {}

			sc::result react(const sc::make_choice &)
			{
				switch(context< paramtree_editor >().current_node_type())
				{
					case ParamType::Enumeration:
					return transit< enumeration >();

					case ParamType::Random:
					return transit< random >();

					case ParamType::Repeat:
					return transit< repeat >();

					default:
					return transit< general >();
				}
			}
		};

	}
}


#endif


