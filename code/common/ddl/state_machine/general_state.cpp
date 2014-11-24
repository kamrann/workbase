// general_state.cpp

#include "general_state.h"


namespace ddl {
	namespace fsm {

		std::string general::get_prompt() const
		{
			auto location = context< paramtree_editor >().nav.where();
			return location.to_string() + ": ";
		}

	}
}





