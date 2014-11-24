// enum_specifiers.h

#ifndef __WB_DDL_ENUM_SPECIFIERS_H
#define __WB_DDL_ENUM_SPECIFIERS_H

#include "defn_node.h"
#include "enum_defn_node.h"
#include "default_specifier.h"
#include "range_specifier.h"
#include "../values/navigator.h"


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

	public:
		enum_defn_node::enum_set_t enum_values_;
	};

	class define_enum_func
	{
	public:
		define_enum_func(enum_defn_node::enum_values_fn_t fn):
			fn_{ fn }
		{}

	public:
		enum_defn_node::enum_values_fn_t fn_;
	};


	class enum_specifier
	{
	public:
		enum_specifier(enum_defn_node&& node): node_(node)
		{}

	public:
		enum_specifier operator() (spc_range< size_t > const& rg)
		{
			auto result = std::move(*this);
			result.node_.minsel(rg.min_ ? *rg.min_ : 0);
			result.node_.maxsel(rg.max_);
			return result;
		}

		enum_specifier operator() (spc_default< enum_defn_node::value_t > const& dft)
		{
			auto result = std::move(*this);
			result.node_.default(dft.val_);
			return result;
		}

		enum_specifier operator() (define_enum_fixed const& fixed)
		{
			auto result = std::move(*this);
			// Need to capture by value
			auto vals = fixed.enum_values_;
			result.node_.enumvalues_fn([vals](navigator)
			{
				return vals;
			});
			return result;
		}

		enum_specifier operator() (define_enum_func const& func)
		{
			auto result = std::move(*this);
			result.node_.enumvalues_fn(func.fn_);
			return result;
		}

		operator defn_node()
		{
			return std::move(node_);
		}

	private:
		enum_defn_node node_;
	};

}


#endif

