// defn_node.cpp

#include "defn_node.h"
#include "boolean_defn_node.h"
#include "integer_defn_node.h"
#include "realnum_defn_node.h"
#include "string_defn_node.h"
#include "enum_defn_node.h"
#include "list_defn_node.h"
#include "composite_defn_node.h"
#include "conditional_defn_node.h"


namespace ddl {

	defn_node::defn_node()
	{
		// TODO:
	}

	defn_node::defn_node(boolean_defn_node const& bn):
		storage_{ std::make_shared< defn_holder >(bn.ptr()) }
	{
		
	}

	defn_node::defn_node(integer_defn_node const& in):
		storage_{ std::make_shared< defn_holder >(in.ptr()) }
	{

	}
	
	defn_node::defn_node(realnum_defn_node const& rn):
		storage_{ std::make_shared< defn_holder >(rn.ptr()) }
	{

	}
	
	defn_node::defn_node(string_defn_node const& sn):
		storage_{ std::make_shared< defn_holder >(sn.ptr()) }
	{

	}
	
	defn_node::defn_node(enum_defn_node const& en):
		storage_{ std::make_shared< defn_holder >(en.ptr()) }
	{

	}
	
	defn_node::defn_node(list_defn_node const& ln):
		storage_{ std::make_shared< defn_holder >(ln.ptr()) }
	{

	}
	
	defn_node::defn_node(composite_defn_node const& cn):
		storage_{ std::make_shared< defn_holder >(cn.ptr()) }
	{

	}

	defn_node::defn_node(conditional_defn_node const& cn):
		storage_{ std::make_shared< defn_holder >(cn.ptr()) }
	{

	}

	defn_node::operator bool() const
	{
		return (bool)storage_;
	}

	void defn_node::set_id(node_id id)
	{
		storage_->id_ = id;
	}

	node_id defn_node::get_id() const
	{
		return storage_->id_;
	}

}





