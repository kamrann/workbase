// integer_defn_node.h
/*
Options:
range
default
*/

#ifndef __WB_DDL_INTEGER_DEFN_NODE_H
#define __WB_DDL_INTEGER_DEFN_NODE_H

#include "integer_defn_node_impl.h"
#include "defn_node_ptr.h"

#include <cstdint>
#include <memory>


namespace ddl {

	class integer_defn_node
	{
	public:
		typedef detail::integer_defn_node_t impl_t;
		typedef impl_t::value_t value_t;

	public:
		integer_defn_node();
		integer_defn_node(detail::integer_defn_node_ptr ptr);

	public:
		void default(value_t val);
		value_t default() const;
		void min(boost::optional< value_t > val);
		boost::optional< value_t > min() const;
		void max(boost::optional< value_t > val);
		boost::optional< value_t > max() const;

	private:
		detail::integer_defn_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class defn_node;
	};

}


#endif


