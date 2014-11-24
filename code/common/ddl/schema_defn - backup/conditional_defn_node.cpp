// conditional_defn_node.cpp

#include "conditional_defn_node.h"
#include "../values/navigator.h"


namespace ddl {

	namespace detail {

		conditional_defn_node_t::conditional_defn_node_t()
		{

		}

	}

	conditional_defn_node::conditional_defn_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	conditional_defn_node::conditional_defn_node(detail::conditional_defn_node_ptr ptr):
		obj_{ ptr }
	{}

	void conditional_defn_node::push_condition(cnd::condition cond, defn_node defn)
	{
		obj_->components_.push_back({ cond, defn });
	}

	void conditional_defn_node::set_default(defn_node defn)
	{
		obj_->default_ = defn;
	}

	defn_node conditional_defn_node::evaluate(ref_resolver const& rr, navigator nav) const
	{
		// TODO: Decide how to deal with untestable multi-part conditions.
		// Currently just calling the whole condition untestable at the first untestable component, which is
		// not very consistent.

		for(auto const& comp : obj_->components_)
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

		if(obj_->default_)
		{
			return obj_->default_;
		}

		return{};
	}

}




