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
			//result.enum_values_.insert(opt);
			result.enum_values_.insert(std::end(result.enum_values_), opt);
			return result;
		}

		define_enum_fixed operator() (std::string opt_str)
		{
			auto result = *this;
			auto opt = enum_defn_node::enum_value_t{ opt_str, boost::any{} };
			result.enum_values_.insert(std::end(result.enum_values_), opt);
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

		define_enum_func(enum_defn_node::enum_values_str_fn_t fn)
		{
			fn_ = enum_defn_node::enum_values_fn_t{
				[fn](ddl::navigator nav)
				{
					auto enum_strings = fn(nav);
					enum_defn_node::enum_set_t enum_values;
					for(auto const& str : enum_strings)
					{
						enum_values.push_back(enum_defn_node::enum_value_t{ str, boost::any{} });
					}
					return enum_values;
				}
			};
			fn_.add_dependency(fn);
		}

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

		enum_specifier operator() (spc_default< enum_defn_node::str_value_t > const& dft)
		{
			auto result = std::move(*this);
			enum_defn_node::value_t vals;
			for(auto const& str : dft.val_)
			{
				vals.push_back(enum_defn_node::enum_value_t{ str, boost::any{} });
			}
			result.node_.default(vals);
			return result;
		}

		enum_specifier operator() (define_enum_fixed const& fixed)
		{
			auto result = std::move(*this);
			// Need to capture by value
			auto vals = fixed.enum_values_;
			auto fn = enum_defn_node::enum_values_fn_t{
				[vals](navigator)
				{
					return vals;
				}
			};
			result.node_.enumvalues_fn(fn);
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

