// conditional_defn_node.cpp

#include "conditional_defn_node.h"
#include "../values/navigator.h"


namespace ddl {

	conditional_defn_node::conditional_defn_node():
		components_{},
		default_{}
	{

	}

	void conditional_defn_node::push_condition(cnd::condition cond, defn_node defn)
	{
		components_.push_back({ cond, defn });
	}

	void conditional_defn_node::set_default(defn_node defn)
	{
		default_ = defn;
	}

	defn_node conditional_defn_node::evaluate(ref_resolver const& rr, navigator nav) const
	{
		// TODO: Decide how to deal with untestable multi-part conditions.
		// Currently just calling the whole condition untestable at the first untestable component, which is
		// not very consistent.

		for(auto const& comp : components_)
		{
			auto result = cnd::test_condition(comp.cond, rr, nav);

			if(result == cnd::ConditionResult::Untestable)
			{
				return{};
			}

			if(result == cnd::ConditionResult::Pass)
			{
				return comp.nd;
			}
		}

		if(default_)
		{
			return default_;
		}

		return{};
	}

}




