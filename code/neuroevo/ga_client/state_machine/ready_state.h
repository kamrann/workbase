// ready_state.h

#ifndef __WB_GA_CLIENT_CONTROL_READY_STATE_H
#define __WB_GA_CLIENT_CONTROL_READY_STATE_H

#include "clsm/clsm.h"
#include "inactive_state.h"


namespace ga_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct init_cmd{};
		struct repeat_cmd{};

		struct active;

		struct ready:
			clsm::clsm_state_base < ready, inactive >,
			clsm::i_cmd_state
		{
			typedef clsm_state_base base;

			ready(my_context ctx): base(ctx)
			{
				reg_cmd< init_cmd >([]()->qi::rule< clsm::iter_t, init_cmd(), clsm::skip_t >{ return qi::lit("init")[ phx::nothing ]; });
				reg_cmd< repeat_cmd >([]()->qi::rule< clsm::iter_t, repeat_cmd(), clsm::skip_t >{ return qi::lit("repeat")[phx::nothing]; });

//				qi::rule< clsm::iter_t, init_cmd(), clsm::skip_t > rule = qi::lit("init")[phx::nothing];
//				reg_cmd< init_cmd >(wrap_rule_temp(rule));
			}

			void on_repeat(clsm::ev_cmd< repeat_cmd > const& cmd);

			typedef mpl::list<
				sc::transition< clsm::ev_cmd< init_cmd >, active >
				, sc::in_state_reaction< clsm::ev_cmd< repeat_cmd >, ready, &ready::on_repeat >
			> my_reactions;

			typedef mpl::copy<
				my_reactions,
				mpl::inserter< base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			using base::react;

			virtual std::string get_prompt() const override;
		};

	}
}


#endif


