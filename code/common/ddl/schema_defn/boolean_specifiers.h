// boolean_specifiers.h

#ifndef __WB_DDL_BOOLEAN_SPECIFIERS_H
#define __WB_DDL_BOOLEAN_SPECIFIERS_H

#include "defn_node.h"
#include "boolean_defn_node.h"
#include "default_specifier.h"


namespace ddl {

	class boolean_specifier
	{
	public:
		boolean_specifier(boolean_defn_node&& node): node_(node)
		{}

	public:
		boolean_specifier operator() (spc_default< boolean_defn_node::value_t > const& dft)
		{
			auto result = std::move(*this);
			result.node_.default(dft.val_);
			return result;
		}

		operator defn_node()
		{
			return std::move(node_);
		}

	private:
		boolean_defn_node node_;
	};

}


#endif

