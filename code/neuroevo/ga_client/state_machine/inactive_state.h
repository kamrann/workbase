// inactive_state.h

#ifndef __WB_GA_CLIENT_CONTROL_INACTIVE_STATE_H
#define __WB_GA_CLIENT_CONTROL_INACTIVE_STATE_H

#include "clsm/clsm.h"
#include "root_state.h"


namespace ga_control {
	namespace fsm {

		namespace qi = boost::spirit::qi;
		namespace phx = boost::phoenix;

		struct modify_cmd{};
		struct dbplot_cmd{};

		struct is_ready_choice;
		struct editing;

		struct inactive:
			clsm::clsm_state_base < inactive, root, is_ready_choice >
		{
			typedef clsm_state_base base;

			inactive(my_context ctx): base(ctx)
			{
				reg_cmd< modify_cmd >([]()->qi::rule< clsm::iter_t, modify_cmd(), clsm::skip_t >{ return qi::lit("modify")[phx::nothing]; });
				reg_cmd< dbplot_cmd >([]()->qi::rule< clsm::iter_t, dbplot_cmd(), clsm::skip_t >{ return qi::lit("dbplot")[phx::nothing]; });
			}

			void on_dbplot(clsm::ev_cmd< dbplot_cmd > const& cmd);

			typedef mpl::list<
				sc::transition< clsm::ev_cmd< modify_cmd >, editing >
				, sc::in_state_reaction< clsm::ev_cmd< dbplot_cmd >, inactive, &inactive::on_dbplot >
			> my_reactions;

			typedef mpl::copy<
				my_reactions,
				mpl::inserter< base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			using base::react;
		};

	}
}


#endif


