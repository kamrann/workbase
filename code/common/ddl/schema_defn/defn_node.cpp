// defn_node.cpp

#include "defn_node.h"
#include "defn_node_holder.h"


namespace ddl {

	defn_node::defn_node():
		storage_{}
	{
	}

	defn_node::defn_node(boolean_defn_node const& bn):
		storage_{ std::make_shared< detail::defn_holder >(bn) }
	{
		
	}

	defn_node::defn_node(integer_defn_node const& in):
		storage_{ std::make_shared< detail::defn_holder >(in) }
	{

	}
	
	defn_node::defn_node(realnum_defn_node const& rn):
		storage_{ std::make_shared< detail::defn_holder >(rn) }
	{

	}
	
	defn_node::defn_node(string_defn_node const& sn):
		storage_{ std::make_shared< detail::defn_holder >(sn) }
	{

	}
	
	defn_node::defn_node(enum_defn_node const& en):
		storage_{ std::make_shared< detail::defn_holder >(en) }
	{

	}
	
	defn_node::defn_node(list_defn_node const& ln):
		storage_{ std::make_shared< detail::defn_holder >(ln) }
	{

	}
	
	defn_node::defn_node(composite_defn_node const& cn):
		storage_{ std::make_shared< detail::defn_holder >(cn) }
	{

	}

	defn_node::defn_node(conditional_defn_node const& cn):
		storage_{ std::make_shared< detail::defn_holder >(cn) }
	{

	}

	defn_node::operator bool() const
	{
		return (bool)storage_;
	}

/*	void defn_node::set_id(node_id id)
	{
		storage_->id_ = id;
	}
*/

	struct get_id_visitor:
		public boost::static_visitor < node_id >
	{
		template < typename T >
		inline result_type operator() (T const& v) const
		{
			return v.id_;
		}
	};

	node_id defn_node::get_id() const
	{
		auto vis = get_id_visitor{};
		return apply_visitor(vis);
	}

}





