// boolean_sch_node.cpp

#include "boolean_sch_node.h"


namespace ddl {

	namespace detail {

		boolean_sch_node_t::boolean_sch_node_t():
			default_{ false }
		{

		}

	}

	boolean_sch_node::boolean_sch_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	boolean_sch_node::boolean_sch_node(detail::boolean_sch_node_ptr ptr):
		obj_{ ptr }
	{}

	void boolean_sch_node::default(value_t val)
	{
		obj_->default_ = val;
	}

	boolean_sch_node::value_t boolean_sch_node::default() const
	{
		return obj_->default_;
	}

}




