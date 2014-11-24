// boolean_defn_node.cpp

#include "boolean_defn_node.h"


namespace ddl {

	boolean_defn_node::boolean_defn_node():
		default_{ false }
	{

	}

	void boolean_defn_node::default(value_t val)
	{
		default_ = val;
	}

	boolean_defn_node::value_t boolean_defn_node::default() const
	{
		return default_;
	}

}




