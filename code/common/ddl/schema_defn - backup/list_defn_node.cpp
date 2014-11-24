// list_defn_node.cpp

#include "list_defn_node.h"


namespace ddl {

	namespace detail {

		list_defn_node_t::list_defn_node_t():
			entry_schema_{},
			min_entries_{ 0 },
			max_entries_{}
		{

		}

	}

	list_defn_node::list_defn_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	list_defn_node::list_defn_node(detail::list_defn_node_ptr ptr):
		obj_{ ptr }
	{}

	void list_defn_node::entrydefn(defn_node val)
	{
		obj_->entry_schema_ = val;
	}

	defn_node list_defn_node::entrydefn() const
	{
		return obj_->entry_schema_;
	}

	void list_defn_node::minentries(size_t val)
	{
		obj_->min_entries_ = val;
	}

	size_t list_defn_node::minentries() const
	{
		return obj_->min_entries_;
	}

	void list_defn_node::maxentries(boost::optional< size_t > val)
	{
		obj_->max_entries_ = val;
	}

	boost::optional< size_t > list_defn_node::maxentries() const
	{
		return obj_->max_entries_;
	}


}




