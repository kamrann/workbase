// realnum_defn_node.cpp

#include "realnum_defn_node.h"


namespace ddl {

	realnum_defn_node::realnum_defn_node():
		min_{},
		max_{},
		default_{ 0.0 }
	{

	}

	void realnum_defn_node::default(value_t val)
	{
		default_ = val;
	}

	realnum_defn_node::value_t realnum_defn_node::default() const
	{
		return default_;
	}

	void realnum_defn_node::min(boost::optional< value_t > val)
	{
		min_ = val;
	}

	boost::optional< realnum_defn_node::value_t > realnum_defn_node::min() const
	{
		return min_;
	}

	void realnum_defn_node::max(boost::optional< value_t > val)
	{
		max_ = val;
	}

	boost::optional< realnum_defn_node::value_t > realnum_defn_node::max() const
	{
		return max_;
	}

}




