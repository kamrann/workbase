// integer_sch_node.h
/*
Options:
range
default
*/

#ifndef __WB_DDL_INTEGER_SCHEMA_NODE_H
#define __WB_DDL_INTEGER_SCHEMA_NODE_H

#include "integer_sch_node_impl.h"
#include "sch_node_ptr.h"

#include <cstdint>
#include <memory>


namespace ddl {

	class integer_sch_node
	{
	public:
		typedef detail::integer_sch_node_t impl_t;
		typedef impl_t::value_t value_t;

	public:
		integer_sch_node();
		integer_sch_node(detail::integer_sch_node_ptr ptr);

	public:
		void default(value_t val);
		value_t default() const;
		void min(boost::optional< value_t > val);
		boost::optional< value_t > min() const;
		void max(boost::optional< value_t > val);
		boost::optional< value_t > max() const;

	private:
		detail::integer_sch_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class sch_node;
	};

}


#endif


