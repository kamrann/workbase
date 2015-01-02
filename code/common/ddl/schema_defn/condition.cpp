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

		ConditionResult equal::test(ref_resolver rr, navigator nav) const
		{
			auto ref_value = unique_reference(rr(ref(), nav));
			if(ref_value)
			{
				// TODO:
				auto en = nav.tree_->node_attribs(ref_value.nd).data.as_enum_str();
				return en.size() == 1 && en.front() == val() ? ConditionResult::Pass : ConditionResult::Fail;
			}
			else
			{
				return ConditionResult::Untestable;
			}
		}

		dep_list equal::deps() const
		{
			return{ ref_ };
		}


		is_selection::is_selection(node_ref ref):
			ref_(ref)
		{}

		ConditionResult is_selection::test(ref_resolver rr, navigator nav) const
		{
			auto ref_value = unique_reference(rr(ref_, nav));
			if(ref_value)
			{
				auto en = nav.tree_->node_attribs(ref_value.nd).data.as_enum();
				return !en.empty() ? ConditionResult::Pass : ConditionResult::Fail;
			}
			else
			{
				return ConditionResult::Untestable;
			}
		}

		dep_list is_selection::deps() const
		{
			return{ ref_ };
		}


		custom_condition::custom_condition(cust_cond_fn_t fn):
			fn_{ fn }
		{}

		ConditionResult custom_condition::test(ref_resolver rr, navigator nav) const
		{
			return fn_(rr, nav);
		}


		struct cond_visitor:
			public boost::static_visitor < ConditionResult >
		{
			cond_visitor(ref_resolver const& rr, navigator const& nav):
				rr_(rr),
				nav_(nav)
			{}

			template < typename Cond >
			result_type operator() (Cond const& cond) const
			{
				return cond.test(rr_, nav_);
			}

			ref_resolver const& rr_;
			navigator const& nav_;
		};

		ConditionResult test_condition(condition const& cond, ref_resolver const& rr, navigator nav)
		{
			cond_visitor vis{ rr, nav };
			return boost::apply_visitor(vis, cond);
		}


		struct cond_dep_visitor:
			public boost::static_visitor < dep_list >
		{
			template < typename T >
			result_type operator() (T const& val) const
			{
				return val.deps();
			}
		};

		dep_list condition_dependencies(condition const& cond)
		{
			auto vis = cond_dep_visitor{};
			return boost::apply_visitor(vis, cond);
		}

	}
}



