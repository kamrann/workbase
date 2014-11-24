// boolean_defn_node.h
/*
Options:
default
*/

#ifndef __WB_DDL_BOOLEAN_DEFN_NODE_H
#define __WB_DDL_BOOLEAN_DEFN_NODE_H

#include "defn_node_base_impl.h"



namespace ddl {

	class boolean_defn_node:
		public detail::defn_node_base_impl
	{
	public:
		typedef bool value_t;

	public:
		boolean_defn_node();

	public:
		void default(value_t val);
		value_t default() const;

	private:
		// TODO: Ideally everything, even a default, could be a function of some value elsewhere in the
		// hierarchy, and so should be able to be specified by a named function.
		value_t default_;

		friend class defn_node;
	};

}


#endif

