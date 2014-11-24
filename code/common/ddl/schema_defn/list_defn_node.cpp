// list_defn_node.cpp

#include "list_defn_node.h"


namespace ddl {

	list_defn_node::list_defn_node():
		entry_schema_{},
		min_entries_{ 0 },
		max_entries_{}
	{

	}

	void list_defn_node::entrydefn(defn_node val)
	{
		entry_schema_ = val;
	}

	defn_node list_defn_node::entrydefn() const
	{
		return entry_schema_;
	}

	void list_defn_node::minentries(size_t val)
	{
		min_entries_ = val;
	}

	size_t list_defn_node::minentries() const
	{
		return min_entries_;
	}

	void list_defn_node::maxentries(boost::optional< size_t > val)
	{
		max_entries_ = val;
	}

	boost::optional< size_t > list_defn_node::maxentries() const
	{
		return max_entries_;
	}


}




