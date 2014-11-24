// integer_defn_node.cpp

#include "integer_defn_node.h"


namespace ddl {

	integer_defn_node::integer_defn_node():
		min_{},
		max_{},
		default_{ 0 }
	{

	}

	void integer_defn_node::default(value_t val)
	{
		default_ = val;
	}

	integer_defn_node::value_t integer_defn_node::default() const
	{
		return default_;
	}

	void integer_defn_node::min(boost::optional< value_t > val)
	{
		min_ = val;
	}

	boost::optional< integer_defn_node::value_t > integer_defn_node::min() const
	{
		return min_;
	}

	void integer_defn_node::max(boost::optional< value_t > val)
	{
		max_ = val;
	}

	boost::optional< integer_defn_node::value_t > integer_defn_node::max() const
	{
		return max_;
	}

}




