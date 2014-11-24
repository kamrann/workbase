// composite_sch_node.cpp

#include "composite_sch_node.h"


namespace ddl {

	namespace detail {

		composite_sch_node_t::composite_sch_node_t()
		{

		}

	}

	composite_sch_node::composite_sch_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	composite_sch_node::composite_sch_node(detail::composite_sch_node_ptr ptr):
		obj_{ ptr }
	{}

	void composite_sch_node::add(node_name id, sch_node child)
	{
		// TODO: check
		obj_->children_[id] = child;
	}

	std::map< node_name, sch_node > composite_sch_node::children() const
	{
		return obj_->children_;
	}

	bool composite_sch_node::has_child(node_name id) const
	{
		return obj_->children_.find(id) != obj_->children_.end();
	}

}




