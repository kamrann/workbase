// boolean_specifiers.h

#ifndef __WB_DDL_BOOLEAN_SPECIFIERS_H
#define __WB_DDL_BOOLEAN_SPECIFIERS_H

#include "boolean_defn_node.h"


namespace ddl {

	class enum_specifier
	{
	public:
		define_enum_fixed(/* TODO: min/max sel */)
		{}

	public:
		define_enum_fixed operator() (enum_defn_node::enum_value_t opt)
		{
			auto result = *this;
			result.enum_values_.insert(opt);
			return result;
		}

		operator enum_defn_node() const
		{
			enum_defn_node nd;
			// Need to capture by value
			auto vals = enum_values_;
			nd.enumvalues_fn([vals]()
			{
				return vals;
			});
			return nd;
		}

	private:
		enum_defn_node::enum_set_t enum_values_;
	};

}


#endif

