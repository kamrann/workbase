// conditional_defn_node.h

#ifndef __WB_DDL_CONDITIONAL_DEFN_NODE_H
#define __WB_DDL_CONDITIONAL_DEFN_NODE_H

#include "conditional_defn_node_impl.h"
#include "defn_node_ptr.h"
#include "condition.h"

#include <vector>


namespace ddl {

	class conditional_defn_node
	{
	public:
		typedef detail::conditional_defn_node_t impl_t;

	public:
		conditional_defn_node();
		conditional_defn_node(detail::conditional_defn_node_ptr ptr);

	public:
		void push_condition(cnd::condition cond, defn_node defn);
		void set_default(defn_node defn);
		defn_node evaluate(ref_resolver const& rr, navigator nav) const;

	private:
		detail::conditional_defn_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class defn_node;
	};

}


#endif


