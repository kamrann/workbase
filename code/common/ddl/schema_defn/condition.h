// condition.h

#ifndef __WB_DDL_CONDITION_H
#define __WB_DDL_CONDITION_H

#include "../ddl_types.h"
#include "../node_ref.h"

#include <boost/variant.hpp>


namespace ddl {
	namespace cnd {

		typedef std::string cnd_value;	// TODO:

		class equal
		{
		public:
			equal(node_ref ref, cnd_value val);

			node_ref const& ref() const;
			cnd_value const& val() const;

		private:
			node_ref ref_;
			cnd_value val_;
		};


		typedef boost::variant <
			equal
		>
		condition;

		enum class ConditionResult {
			Pass,
			Fail,
			Untestable,
		};

		ConditionResult test_condition(condition const& cond, ref_resolver const& rr, navigator nav);

	}
}


#endif


