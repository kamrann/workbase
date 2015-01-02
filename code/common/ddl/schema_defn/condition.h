// condition.h

#ifndef __WB_DDL_CONDITION_H
#define __WB_DDL_CONDITION_H

#include "../ddl_types.h"
#include "../node_ref.h"
#include "../sd_node_ref.h"
#include "../dependencies.h"

#include <boost/variant.hpp>

#include <string>
#include <functional>


namespace ddl {
	namespace cnd {

		typedef std::string cnd_value;	// TODO:

		enum class ConditionResult {
			Pass,
			Fail,
			Untestable,
		};


		class equal
		{
		public:
			equal(node_ref ref, cnd_value val);

			node_ref const& ref() const;
			cnd_value const& val() const;

			ConditionResult test(ref_resolver, navigator) const;

			dep_list deps() const;

		private:
			node_ref ref_;
			cnd_value val_;
		};

		class is_selection
		{
		public:
			is_selection(node_ref ref);	// TODO: more customization, eg. single sel, 1 or more, etc

			ConditionResult test(ref_resolver, navigator) const;

			dep_list deps() const;

		private:
			node_ref ref_;
		};

		class custom_condition:
			public dependent_component
		{
		public:
			typedef std::function< ConditionResult(ref_resolver, navigator) > cust_cond_fn_t;

		public:
			custom_condition(cust_cond_fn_t fn);

		public:
			ConditionResult test(ref_resolver, navigator) const;

		private:
			cust_cond_fn_t fn_;
		};
		

		typedef boost::variant <
			equal
			, is_selection
			, custom_condition
		>
		condition;

		ConditionResult test_condition(condition const& cond, ref_resolver const& rr, navigator nav);
		dep_list condition_dependencies(condition const& cond);

	}
}


#endif


