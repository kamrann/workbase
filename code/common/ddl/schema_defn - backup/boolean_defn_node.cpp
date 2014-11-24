// boolean_defn_node.cpp

#include "boolean_defn_node.h"


namespace ddl {

	namespace detail {

		boolean_defn_node_t::boolean_defn_node_t():
			default_{ false }
		{

		}

	}

	boolean_defn_node::boolean_defn_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	boolean_defn_node::boolean_defn_node(detail::boolean_defn_node_ptr ptr):
		obj_{ ptr }
	{}

	void boolean_defn_node::default(value_t val)
	{
		obj_->default_ = val;
	}

	boolean_defn_node::value_t boolean_defn_node::default() const
	{
		return obj_->default_;
	}

}




