// list_defn_node.h
/*
Options:
min/max entries
entry schema
*/

#ifndef __WB_DDL_LIST_DEFN_NODE_H
#define __WB_DDL_LIST_DEFN_NODE_H

#include "list_defn_node_impl.h"
#include "defn_node_ptr.h"

#include <boost/optional.hpp>

#include <memory>


namespace ddl {

	class list_defn_node
	{
	public:
		typedef detail::list_defn_node_t impl_t;

	public:
		list_defn_node();
		list_defn_node(detail::list_defn_node_ptr ptr);

	public:
		void entrydefn(defn_node item);
		defn_node entrydefn() const;
		void minentries(size_t val);
		size_t minentries() const;
		void maxentries(boost::optional< size_t > val);
		boost::optional< size_t > maxentries() const;

	private:
		detail::list_defn_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class defn_node;
	};

}


#endif


