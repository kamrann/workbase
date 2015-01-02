// random_state.h

#ifndef __WB_PARAMTREE_FSM_RANDOM_STATE_H
#define __WB_PARAMTREE_FSM_RANDOM_STATE_H

#include "base_node_state.h"


namespace prm {

	namespace fsm {

		struct random:
			clsm::clsm_state_base < random, base_node_state >,
			clsm::i_cmd_state
		{
			typedef clsm_state_base base;

			random(my_context ctx): base(ctx)
			{
				reg_cmd< prm::random >(wrap_grammar_prefixed< random_parser< clsm::iter_t >::lax >("set"));
			}

			void on_set(clsm::ev_cmd< prm::random > const& cmd);

			typedef mpl::list<
				sc::in_state_reaction< clsm::ev_cmd< prm::random >, random, &random::on_set >
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


