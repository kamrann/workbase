// composite_defn_node.cpp

#include "composite_defn_node.h"


namespace ddl {

	namespace detail {

		composite_defn_node_t::composite_defn_node_t()
		{

		}

	}

	composite_defn_node::composite_defn_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	composite_defn_node::composite_defn_node(detail::composite_defn_node_ptr ptr):
		obj_{ ptr }
	{}

	void composite_defn_node::add(node_name id, defn_node child)
	{
		// TODO: check
		obj_->children_[id] = child;
	}

	std::map< node_name, defn_node > composite_defn_node::children() const
	{
		return obj_->children_;
	}

}




