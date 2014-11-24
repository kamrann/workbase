// composite_defn_node.cpp

#include "composite_defn_node.h"


namespace ddl {

	composite_defn_node::composite_defn_node():
		children_{}
	{

	}

	void composite_defn_node::add(node_name id, defn_node child)
	{
		// TODO: check
		children_[id] = child;
	}

	std::map< node_name, defn_node > composite_defn_node::children() const
	{
		return children_;
	}

}




