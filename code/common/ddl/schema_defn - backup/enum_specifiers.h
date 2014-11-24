// enum_specifiers.h

#ifndef __WB_DDL_ENUM_SPECIFIERS_H
#define __WB_DDL_ENUM_SPECIFIERS_H

#include "enum_defn_node.h"


namespace ddl {

	class define_enum_fixed
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

	class define_enum_func
	{
	public:
		define_enum_func(/* TODO: min/max sel */)
		{}

	public:
		// TODO: shouldn't be using detail here...
		enum_defn_node operator() (detail::enum_defn_node_t::enum_values_fn_t fn)
		{
			enum_defn_node nd;
			nd.enumvalues_fn(fn);
			return nd;
		}
	};

}


#endif

