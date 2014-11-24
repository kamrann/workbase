// string_defn_node.h
/*
Options:
min/max length
charset
default
*/

#ifndef __WB_DDL_STRING_DEFN_NODE_H
#define __WB_DDL_STRING_DEFN_NODE_H

#include "defn_node_base_impl.h"

#include <boost/optional.hpp>

#include <string>


namespace ddl {

	class string_defn_node:
		public detail::defn_node_base_impl
	{
	public:
		typedef std::string value_t;
		typedef std::string charset_t;

	public:
		string_defn_node();

	public:
		void default(value_t val);
		value_t default() const;
		void minlength(size_t len);
		size_t minlength() const;
		void maxlength(boost::optional< size_t > len);
		boost::optional< size_t > maxlength() const;
		void charset(charset_t cs);
		charset_t charset() const;

	private:
		size_t min_len_;
		boost::optional< size_t > max_len_;
		charset_t charset_;
		value_t default_;

		friend class defn_node;
	};

}


#endif


