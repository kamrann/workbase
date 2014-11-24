// boolean_defn_node.h
/*
Options:
default
*/

#ifndef __WB_DDL_BOOLEAN_DEFN_NODE_H
#define __WB_DDL_BOOLEAN_DEFN_NODE_H

#include "boolean_defn_node_impl.h"
#include "defn_node_ptr.h"

#include <memory>


namespace ddl {

	class boolean_defn_node
	{
	public:
		typedef detail::boolean_defn_node_t impl_t;
		typedef impl_t::value_t value_t;

	public:
		boolean_defn_node();
		boolean_defn_node(detail::boolean_defn_node_ptr ptr);

	public:
		void default(value_t val);
		value_t default() const;

	private:
		detail::boolean_defn_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class defn_node;
	};

}


#endif

