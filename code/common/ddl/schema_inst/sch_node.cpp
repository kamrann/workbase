// sch_sch_node.cpp

#include "sch_node.h"
#include "boolean_sch_node.h"
#include "integer_sch_node.h"
#include "realnum_sch_node.h"
#include "string_sch_node.h"
#include "enum_sch_node.h"
#include "list_sch_node.h"
#include "composite_sch_node.h"
#include "conditional_sch_node.h"


namespace ddl {

	sch_node::sch_node()
	{

	}

	sch_node::sch_node(boolean_sch_node const& bn):
		storage_{ bn.ptr() }
	{
		
	}

	sch_node::sch_node(integer_sch_node const& in):
		storage_{ in.ptr() }
	{

	}
	
	sch_node::sch_node(realnum_sch_node const& rn):
		storage_{ rn.ptr() }
	{

	}
	
	sch_node::sch_node(string_sch_node const& sn):
		storage_{ sn.ptr() }
	{

	}
	
	sch_node::sch_node(enum_sch_node const& en):
		storage_{ en.ptr() }
	{

	}
	
	sch_node::sch_node(list_sch_node const& ln):
		storage_{ ln.ptr() }
	{

	}
	
	sch_node::sch_node(composite_sch_node const& cn):
		storage_{ cn.ptr() }
	{

	}

	sch_node::sch_node(conditional_sch_node const& cn):
		storage_{ cn.ptr() }
	{

	}

	sch_node::operator bool() const
	{
		// TODO: big hack
		return !(
			storage_.which() == 0 &&
			boost::get< detail::boolean_sch_node_ptr >(storage_) == nullptr
			);
	}

	void sch_node::set_id(node_id id)
	{
		id_ = id;
	}

	node_id sch_node::get_id() const
	{
		return id_;
	}

	template <>
	auto sch_node::get_as< enum_sch_node >() const -> enum_sch_node
	{
		return enum_sch_node{ boost::get< detail::enum_sch_node_ptr >(storage_) };
	}

	// todo: other types

	//template sch_node::get_as < > ;

}





