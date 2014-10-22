// choice_point.h

#ifndef __WB_FSM_CHOICE_POINT_H
#define __WB_FSM_CHOICE_POINT_H

#include <boost/statechart/event.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/custom_reaction.hpp>


namespace boost {
	namespace statechart {

		//////////////
		// See http://www.boost.org/doc/libs/1_56_0/libs/statechart/doc/rationale.html#DynamicConfigurability

		struct make_choice:
			event< make_choice >
		{};

		template < class MostDerived, class Context >
		struct choice_point:
			state< MostDerived, Context >
		{
			typedef state< MostDerived, Context > base_type;
			typedef typename base_type::my_context my_context;
			typedef choice_point my_base;

			typedef custom_reaction< make_choice > reactions;

			choice_point(my_context ctx): base_type(ctx)
			{
				post_event(boost::intrusive_ptr< make_choice >(new make_choice()));
			}
		};

	}
}


#endif


