// string_defn_node.h
/*
Options:
min/max length
charset
default
*/

#ifndef __WB_DDL_STRING_DEFN_NODE_H
#define __WB_DDL_STRING_DEFN_NODE_H

#include "string_defn_node_impl.h"
#include "defn_node_ptr.h"

#include <boost/optional.hpp>

#include <string>
#include <memory>


namespace ddl {

	class string_defn_node
	{
	public:
		typedef detail::string_defn_node_t impl_t;
		typedef impl_t::value_t value_t;
		typedef impl_t::charset_t charset_t;

	public:
		string_defn_node();
		string_defn_node(detail::string_defn_node_ptr ptr);

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
		detail::string_defn_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class defn_node;
	};

}


#endif


