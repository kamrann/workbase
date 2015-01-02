// conditional_defn_node.h

#ifndef __WB_DDL_CONDITIONAL_DEFN_NODE_H
#define __WB_DDL_CONDITIONAL_DEFN_NODE_H

#include "defn_node_base_impl.h"
#include "condition.h"

#include <vector>


namespace ddl {

	class conditional_defn_node:
		public detail::defn_node_base_impl
	{
	public:
		struct component
		{
			cnd::condition cond;
			defn_node nd;
		};

	public:
		conditional_defn_node();

	public:
		void push_condition(cnd::condition cond, defn_node defn);
		void set_default(defn_node defn);
		defn_node evaluate(ref_resolver const& rr, navigator nav) const;

		dep_list deps() const;

	public:
		std::vector< component > components_;
		defn_node default_;

		friend class defn_node;
	};

}


#endif


