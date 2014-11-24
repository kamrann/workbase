// conditional_specifiers.h

#ifndef __WB_DDL_CONDITIONAL_SPECIFIERS_H
#define __WB_DDL_CONDITIONAL_SPECIFIERS_H

#include "defn_node.h"
#include "conditional_defn_node.h"
#include "../values/navigator.h"


namespace ddl {

	class spc_condition
	{
	public:
		spc_condition(cnd::condition cond, defn_node defn):
			cond_{ cond },
			defn_{ defn }
		{}

	public:
		cnd::condition cond_;
		defn_node defn_;
	};

	class spc_default_condition
	{
	public:
		spc_default_condition(defn_node defn):
			defn_{ defn }
		{}

	public:
		defn_node defn_;
	};


	class conditional_specifier
	{
	public:
		conditional_specifier(conditional_defn_node&& node): node_(node)
		{}

	public:
		conditional_specifier operator() (spc_condition const& cond)
		{
			auto result = std::move(*this);
			result.node_.push_condition(cond.cond_, cond.defn_);
			return result;
		}

		conditional_specifier operator() (spc_default_condition const& dft_cond)
		{
			auto result = std::move(*this);
			result.node_.set_default(dft_cond.defn_);
			return result;
		}

		operator defn_node()
		{
			return std::move(node_);
		}

	private:
		conditional_defn_node node_;
	};

}


#endif

