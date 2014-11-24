// random_state.cpp

#include "random_state.h"

#include "params/random_parser.h"


namespace ddl {
	namespace fsm {

		void random::on_set(clsm::ev_cmd< prm::random > const& cmd)
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
				return;// false;
			}

			pd.value = cmd;// .val;

			// TODO: Overkill
			editor.reload_pt_schema();

			editor.output("value set");
		}

		std::string random::get_prompt() const
		{
			auto location = context< paramtree_editor >().acc.where();
			return location.to_string() + " [rdm]: ";
		}

	}
}




