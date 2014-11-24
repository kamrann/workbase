// integer_specifiers.h

#ifndef __WB_DDL_INTEGER_SPECIFIERS_H
#define __WB_DDL_INTEGER_SPECIFIERS_H

#include "defn_node.h"
#include "integer_defn_node.h"
#include "default_specifier.h"
#include "range_specifier.h"
#include "min_specifier.h"


namespace ddl {

	class integer_specifier
	{
	public:
		integer_specifier(integer_defn_node&& node): node_(node)
		{}

	public:
		integer_specifier operator() (spc_default< integer_defn_node::value_t > const& dft)
		{
			auto result = std::move(*this);
			result.node_.default(dft.val_);
			return result;
		}

		integer_specifier operator() (spc_range< integer_defn_node::value_t > const& rg)
		{
			auto result = std::move(*this);
			result.node_.min(rg.min_);
			result.node_.max(rg.max_);
			return result;
		}

		integer_specifier operator() (spc_min< integer_defn_node::value_t > const& min)
		{
			auto result = std::move(*this);
			result.node_.min(min.val_);
			return result;
		}

		operator defn_node()
		{
			return std::move(node_);
		}

	private:
		integer_defn_node node_;
	};

}


#endif

