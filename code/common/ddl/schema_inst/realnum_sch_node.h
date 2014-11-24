// realnum_sch_sch_node.h
/*
Options:
range
default
*/

#ifndef __WB_DDL_REALNUM_SCHEMA_NODE_H
#define __WB_DDL_REALNUM_SCHEMA_NODE_H

#include "realnum_sch_node_impl.h"
#include "sch_node_ptr.h"

#include <memory>


namespace ddl {

	class realnum_sch_node
	{
	public:
		typedef detail::realnum_sch_node_t impl_t;
		typedef impl_t::value_t value_t;

	public:
		realnum_sch_node();
		realnum_sch_node(detail::realnum_sch_node_ptr ptr);

	public:
		void default(value_t val);
		value_t default() const;
		void min(boost::optional< value_t > val);
		boost::optional< value_t > min() const;
		void max(boost::optional< value_t > val);
		boost::optional< value_t > max() const;

	private:
		detail::realnum_sch_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class sch_node;
	};

}


#endif


