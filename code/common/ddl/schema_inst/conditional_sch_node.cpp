// conditional_sch_node.cpp

#include "conditional_sch_node.h"


namespace ddl {

	namespace detail {

		conditional_sch_node_t::conditional_sch_node_t()
		{

		}

	}

	conditional_sch_node::conditional_sch_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	conditional_sch_node::conditional_sch_node(detail::conditional_sch_node_ptr ptr):
		obj_{ ptr }
	{}

	

}




