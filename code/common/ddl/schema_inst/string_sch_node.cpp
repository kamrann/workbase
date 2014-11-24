// string_sch_node.cpp

#include "string_sch_node.h"


namespace ddl {

	namespace detail {

		string_sch_node_t::string_sch_node_t():
			min_len_{ 0 },
			max_len_{},
			charset_{ "" },
			default_{ "" }
		{

		}

	}

	string_sch_node::string_sch_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	string_sch_node::string_sch_node(detail::string_sch_node_ptr ptr):
		obj_{ ptr }
	{}

	void string_sch_node::default(value_t val)
	{
		obj_->default_ = val;
	}

	string_sch_node::value_t string_sch_node::default() const
	{
		return obj_->default_;
	}

	void string_sch_node::minlength(size_t len)
	{
		obj_->min_len_ = len;
	}

	size_t string_sch_node::minlength() const
	{
		return obj_->min_len_;
	}

	void string_sch_node::maxlength(boost::optional< size_t > len)
	{
		obj_->max_len_ = len;
	}

	boost::optional< size_t > string_sch_node::maxlength() const
	{
		return obj_->max_len_;
	}

	void string_sch_node::charset(charset_t cs)
	{
		obj_->charset_ = cs;
	}

	string_sch_node::charset_t string_sch_node::charset() const
	{
		return obj_->charset_;
	}

}




