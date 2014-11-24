// string_defn_node.cpp

#include "string_defn_node.h"


namespace ddl {

	namespace detail {

		string_defn_node_t::string_defn_node_t():
			min_len_{ 0 },
			max_len_{},
			charset_{ "" },
			default_{ "" }
		{

		}

	}

	string_defn_node::string_defn_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	string_defn_node::string_defn_node(detail::string_defn_node_ptr ptr):
		obj_{ ptr }
	{}

	void string_defn_node::default(value_t val)
	{
		obj_->default_ = val;
	}

	string_defn_node::value_t string_defn_node::default() const
	{
		return obj_->default_;
	}

	void string_defn_node::minlength(size_t len)
	{
		obj_->min_len_ = len;
	}

	size_t string_defn_node::minlength() const
	{
		return obj_->min_len_;
	}

	void string_defn_node::maxlength(boost::optional< size_t > len)
	{
		obj_->max_len_ = len;
	}
	
	boost::optional< size_t > string_defn_node::maxlength() const
	{
		return obj_->max_len_;
	}

	void string_defn_node::charset(charset_t cs)
	{
		obj_->charset_ = cs;
	}

	string_defn_node::charset_t string_defn_node::charset() const
	{
		return obj_->charset_;
	}

}




