// list_specifiers.h

#ifndef __WB_DDL_LIST_SPECIFIERS_H
#define __WB_DDL_LIST_SPECIFIERS_H

#include "defn_node.h"
#include "list_defn_node.h"
#include "basic_value_specifier.h"
#include "range_specifier.h"
#include "min_specifier.h"
#include "../values/navigator.h"


namespace ddl {

	typedef spc_basic_value< defn_node > spc_item;

	class list_specifier
	{
	public:
		list_specifier(list_defn_node&& node): node_(node)
		{}

	public:
		list_specifier operator() (spc_item const& item_def)
		{
			auto result = std::move(*this);
			result.node_.entrydefn(item_def.val_);
			return result;
		}

		list_specifier operator() (spc_min< size_t > const& min)
		{
			auto result = std::move(*this);
			result.node_.minentries(min.val_);
			return result;
		}

		list_specifier operator() (spc_range< size_t > const& rg)
		{
			auto result = std::move(*this);
			result.node_.minentries(rg.min_ ? *rg.min_ : 0);
			result.node_.maxentries(rg.max_);
			return result;
		}

		operator defn_node()
		{
			return std::move(node_);
		}

	private:
		list_defn_node node_;
	};

}


#endif

