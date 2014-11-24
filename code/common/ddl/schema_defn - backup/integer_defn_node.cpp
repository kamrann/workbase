// integer_defn_node.cpp

#include "integer_defn_node.h"


namespace ddl {

	namespace detail {

		integer_defn_node_t::integer_defn_node_t():
			min_{},
			max_{},
			default_{ 0 }
		{

		}

	}

	integer_defn_node::integer_defn_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	integer_defn_node::integer_defn_node(detail::integer_defn_node_ptr ptr):
		obj_{ ptr }
	{}

	void integer_defn_node::default(value_t val)
	{
		obj_->default_ = val;
	}

	integer_defn_node::value_t integer_defn_node::default() const
	{
		return obj_->default_;
	}

	void integer_defn_node::min(boost::optional< value_t > val)
	{
		obj_->min_ = val;
	}

	boost::optional< integer_defn_node::value_t > integer_defn_node::min() const
	{
		return obj_->min_;
	}

	void integer_defn_node::max(boost::optional< value_t > val)
	{
		obj_->max_ = val;
	}

	boost::optional< integer_defn_node::value_t > integer_defn_node::max() const
	{
		return obj_->max_;
	}

}




