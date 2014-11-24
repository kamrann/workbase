// list_sch_sch_node.h
/*
Options:
min/max entries
entry schema
*/

#ifndef __WB_DDL_LIST_SCHEMA_NODE_H
#define __WB_DDL_LIST_SCHEMA_NODE_H

#include "list_sch_node_impl.h"
#include "sch_node_ptr.h"

#include <boost/optional.hpp>

#include <memory>


namespace ddl {

	class list_sch_node
	{
	public:
		typedef detail::list_sch_node_t impl_t;

	public:
		list_sch_node();
		list_sch_node(detail::list_sch_node_ptr ptr);

	public:
		void entries(std::vector< sch_node > ent);
		std::vector< sch_node > entries() const;
		void minentries(size_t count);
		size_t minentries() const;
		void maxentries(boost::optional< size_t > count);
		boost::optional< size_t > maxentries() const;

	private:
		detail::list_sch_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class sch_node;
	};

}


#endif


