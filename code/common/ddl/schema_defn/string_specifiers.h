// string_specifiers.h

#ifndef __WB_DDL_STRING_SPECIFIERS_H
#define __WB_DDL_STRING_SPECIFIERS_H

#include "defn_node.h"
#include "string_defn_node.h"
#include "default_specifier.h"
#include "range_specifier.h"
#include "../values/navigator.h"


namespace ddl {

	struct spc_charset
	{
		spc_charset(string_defn_node::charset_t cs): val_{ std::move(cs) }
		{}

		string_defn_node::charset_t val_;
	};
	
	class string_specifier
	{
	public:
		string_specifier(string_defn_node&& node): node_(node)
		{}

	public:
		string_specifier operator() (spc_range< size_t > const& rg)
		{
			auto result = std::move(*this);
			result.node_.minlength(rg.min_ ? *rg.min_ : 0);
			result.node_.maxlength(rg.max_);
			return result;
		}

		string_specifier operator() (spc_default< string_defn_node::value_t > const& dft)
		{
			auto result = std::move(*this);
			result.node_.default(dft.val_);
			return result;
		}

		string_specifier operator() (spc_charset const& cs)
		{
			auto result = std::move(*this);
			result.node_.charset(cs.val_);
			return result;
		}

		operator defn_node()
		{
			return std::move(node_);
		}

	private:
		string_defn_node node_;
	};

}


#endif

