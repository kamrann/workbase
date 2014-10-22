// random_state.cpp

#include "random_state.h"

#include "params/random_parser.h"


namespace prm {
	namespace fsm {

		sc::result random::dispatch(prm::random const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			auto path = acc.where();
			auto& pd = acc.param_here();
			// TODO: Perhaps current schema should be stored, maybe within the tree?
			auto sch = (*editor.provider)[path.unindexed()](acc);

			bool ok = schema::validate_param(cmd/*.val*/, sch);
			if(!ok)
			{
				editor.output("invalid value");
				return discard_event();// false;
			}

			pd.value = cmd;// .val;

			// TODO: Overkill
			editor.reload_pt_schema();

			editor.output("value set");
			return discard_event();
		}

		std::string random::get_prompt() const
		{
			auto location = context< paramtree_editor >().acc.where();
			return location.to_string() + " [rdm]: ";
		}

	}
}




