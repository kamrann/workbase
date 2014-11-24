// condition.cpp

#include "condition.h"
#include "../values/value_node.h"
#include "../values/navigator.h"


namespace ddl {
	namespace cnd {

		equal::equal(node_ref ref, cnd_value val):
			ref_(ref),
			val_(val)
		{}

		node_ref const& equal::ref() const
		{
			return ref_;
		}

		cnd_value const& equal::val() const
		{
			return val_;
		}


		ConditionResult test_condition(condition const& cond, ref_resolver const& rr, navigator nav)
		{
			auto temp = boost::get< equal >(cond);
			auto ref_value = rr(temp.ref(), nav);
			if(ref_value)
			{
				// TODO:
				auto en = ref_value.as_enum();
				return en.size() == 1 && en.front() == temp.val() ? ConditionResult::Pass : ConditionResult::Fail;
			}
			else
			{
				return ConditionResult::Untestable;
			}
		}

	}
}



