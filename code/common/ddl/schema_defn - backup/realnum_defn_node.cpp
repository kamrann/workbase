// realnum_defn_node.cpp

#include "realnum_defn_node.h"


namespace ddl {

	namespace detail {

		realnum_defn_node_t::realnum_defn_node_t():
			min_{},
			max_{},
			default_{ 0.0 }
		{

		}

	}

	realnum_defn_node::realnum_defn_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	realnum_defn_node::realnum_defn_node(detail::realnum_defn_node_ptr ptr):
		obj_{ ptr }
	{}

	void realnum_defn_node::default(value_t val)
	{
		obj_->default_ = val;
	}

	realnum_defn_node::value_t realnum_defn_node::default() const
	{
		return obj_->default_;
	}

	void realnum_defn_node::min(boost::optional< value_t > val)
	{
		obj_->min_ = val;
	}

	boost::optional< realnum_defn_node::value_t > realnum_defn_node::min() const
	{
		return obj_->min_;
	}

	void realnum_defn_node::max(boost::optional< value_t > val)
	{
		obj_->max_ = val;
	}

	boost::optional< realnum_defn_node::value_t > realnum_defn_node::max() const
	{
		return obj_->max_;
	}

}




