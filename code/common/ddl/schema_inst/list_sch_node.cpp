// list_sch_node.cpp

#include "list_sch_node.h"


namespace ddl {

	namespace detail {

		list_sch_node_t::list_sch_node_t():
			entry_schemas_{},
			min_entries_{ 0 },
			max_entries_{}
		{

		}

	}

	list_sch_node::list_sch_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	list_sch_node::list_sch_node(detail::list_sch_node_ptr ptr):
		obj_{ ptr }
	{}

	void list_sch_node::entries(std::vector< sch_node > ent)
	{
		obj_->entry_schemas_ = std::move(ent);
	}
	
	std::vector< sch_node > list_sch_node::entries() const
	{
		return obj_->entry_schemas_;
	}
	
	void list_sch_node::minentries(size_t count)
	{
		obj_->min_entries_ = count;
	}

	size_t list_sch_node::minentries() const
	{
		return obj_->min_entries_;
	}

	void list_sch_node::maxentries(boost::optional< size_t > count)
	{
		obj_->max_entries_ = count;
	}

	boost::optional< size_t > list_sch_node::maxentries() const
	{
		return obj_->max_entries_;
	}

}




