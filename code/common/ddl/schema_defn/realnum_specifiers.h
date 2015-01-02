// realnum_specifiers.h

#ifndef __WB_DDL_REALNUM_SPECIFIERS_H
#define __WB_DDL_REALNUM_SPECIFIERS_H

#include "defn_node.h"
#include "realnum_defn_node.h"
#include "default_specifier.h"
#include "range_specifier.h"
#include "min_specifier.h"


namespace ddl {

	class realnum_specifier
	{
	public:
		realnum_specifier(realnum_defn_node&& node): node_(node)
		{}

	public:
		realnum_specifier operator() (spc_default< realnum_defn_node::value_t > const& dft)
		{
			auto result = std::move(*this);
			result.node_.default(dft.val_);
			return result;
		}

		realnum_specifier operator() (spc_range< realnum_defn_node::value_t > const& rg)
		{
			auto result = std::move(*this);
			result.node_.min(rg.min_);
			result.node_.max(rg.max_);
			return result;
		}

		realnum_specifier operator() (spc_min< realnum_defn_node::value_t > const& min)
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
		realnum_defn_node node_;
	};

}


#endif

