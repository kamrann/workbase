// string_defn_node.cpp

#include "string_defn_node.h"


namespace ddl {

	string_defn_node::string_defn_node():
		min_len_{ 0 },
		max_len_{},
		charset_{ "" },
		default_{ "" }
	{

	}

	void string_defn_node::default(value_t val)
	{
		default_ = val;
	}

	string_defn_node::value_t string_defn_node::default() const
	{
		return default_;
	}

	void string_defn_node::minlength(size_t len)
	{
		min_len_ = len;
	}

	size_t string_defn_node::minlength() const
	{
		return min_len_;
	}

	void string_defn_node::maxlength(boost::optional< size_t > len)
	{
		max_len_ = len;
	}
	
	boost::optional< size_t > string_defn_node::maxlength() const
	{
		return max_len_;
	}

	void string_defn_node::charset(charset_t cs)
	{
		charset_ = cs;
	}

	string_defn_node::charset_t string_defn_node::charset() const
	{
		return charset_;
	}

}




