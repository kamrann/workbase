// list_defn_node.h
/*
Options:
min/max entries
entry schema
*/

#ifndef __WB_DDL_LIST_DEFN_NODE_H
#define __WB_DDL_LIST_DEFN_NODE_H

#include "defn_node_base_impl.h"
#include "defn_node.h"

#include <boost/optional.hpp>


namespace ddl {

	class list_defn_node:
		public detail::defn_node_base_impl
	{
	public:
		list_defn_node();

	public:
		void entrydefn(defn_node item);
		defn_node entrydefn() const;
		void minentries(size_t val);
		size_t minentries() const;
		void maxentries(boost::optional< size_t > val);
		boost::optional< size_t > maxentries() const;

	private:
		defn_node entry_schema_;
		size_t min_entries_;
		boost::optional< size_t > max_entries_;

		friend class defn_node;
	};

}


#endif


