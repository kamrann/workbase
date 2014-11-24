// value_node.cpp

#include "value_node.h"


namespace ddl {

	value_node::value_node():
		storage_{}// std::make_shared< value_holder >()
	{
	
	}

	bool value_node::initialized() const
	{
		return (bool)storage_;
	}

	value_node::operator bool() const
	{
		return initialized();
	}

	value_node& value_node::operator= (boolean_sch_node::value_t const& val)
	{
		if(!initialized())
		{
			storage_ = std::make_shared< value_holder >();
		}
		*storage_ = val;
		return *this;
	}

	value_node& value_node::operator= (integer_sch_node::value_t const& val)
	{
		if(!initialized())
		{
			storage_ = std::make_shared< value_holder >();
		}
		*storage_ = val;
		return *this;
	}

	value_node& value_node::operator= (realnum_sch_node::value_t const& val)
	{
		if(!initialized())
		{
			storage_ = std::make_shared< value_holder >();
		}
		*storage_ = val;
		return *this;
	}

	value_node& value_node::operator= (string_sch_node::value_t const& val)
	{
		if(!initialized())
		{
			storage_ = std::make_shared< value_holder >();
		}
		*storage_ = val;
		return *this;
	}

	value_node& value_node::operator= (enum_sch_node::value_t const& val)
	{
		if(!initialized())
		{
			storage_ = std::make_shared< value_holder >();
		}
		*storage_ = val;
		return *this;
	}

	value_node& value_node::operator= (std::vector< value_node > const& val)
	{
		if(!initialized())
		{
			storage_ = std::make_shared< value_holder >();
		}
		*storage_ = val;
		return *this;
	}

	value_node& value_node::operator= (std::map< node_name, value_node > const& val)
	{
		if(!initialized())
		{
			storage_ = std::make_shared< value_holder >();
		}
		*storage_ = val;
		return *this;
	}

	struct val_assignment_visitor:
		public boost::static_visitor < void >
	{
		val_assignment_visitor(value_node& vnode):
			vnode_{ vnode }
		{}

		template < typename T >
		inline result_type operator() (T const& v)
		{
			vnode_ = v;
		}

		value_node& vnode_;
	};

	value_node& value_node::operator= (value const& val)
	{
		val_assignment_visitor vis{ *this };
		boost::apply_visitor(vis, val);
		return *this;
	}


	bool value_node::is_bool() const
	{
		return initialized() && storage_->which() == 0;
	}

	bool value_node::is_int() const
	{
		return initialized() && storage_->which() == 1;
	}

	bool value_node::is_real() const
	{
		return initialized() && storage_->which() == 2;
	}

	bool value_node::is_string() const
	{
		return initialized() && storage_->which() == 3;
	}

	bool value_node::is_enum() const
	{
		return initialized() && storage_->which() == 4;
	}

	bool value_node::is_list() const
	{
		return initialized() && storage_->which() == 5;
	}

	bool value_node::is_composite() const
	{
		return initialized() && storage_->which() == 6;
	}

	bool value_node::is_leaf() const
	{
		// TODO: Clearer distinction - list with no items? uninitialized node?
		return !is_list() && !is_composite();
	}


	boolean_sch_node::value_t value_node::as_bool() const
	{
		return boost::get< boolean_sch_node::value_t >(*storage_);
	}

	integer_sch_node::value_t value_node::as_int() const
	{
		return boost::get< integer_sch_node::value_t >(*storage_);
	}

	realnum_sch_node::value_t value_node::as_real() const
	{
		return boost::get< realnum_sch_node::value_t >(*storage_);
	}

	string_sch_node::value_t value_node::as_string() const
	{
		return boost::get< string_sch_node::value_t >(*storage_);
	}

	enum_sch_node::value_t value_node::as_enum() const
	{
		return boost::get< enum_sch_node::value_t >(*storage_);
	}

	std::vector< value_node > value_node::as_list() const
	{
		return boost::get< std::vector< value_node > >(*storage_);
	}

	std::map< node_name, value_node > value_node::as_composite() const
	{
		return boost::get< std::map< node_name, value_node > >(*storage_);
	}

	struct make_value_visitor:
		boost::static_visitor< value >
	{
		inline result_type operator() (std::vector< value_node > const&)
		{
			throw std::runtime_error("not leaf node");
		}

		inline result_type operator() (std::map< node_name, value_node > const&)
		{
			throw std::runtime_error("not leaf node");
		}

		template < typename T >
		inline result_type operator() (T const& v)
		{
			return value(v);
		}
	};

	value value_node::as_value() const
	{
		make_value_visitor vis;
		return apply_visitor(vis);
	}

	enum_sch_node::enum_value_t value_node::as_single_enum() const
	{
		return *std::begin(as_enum());
	}

	void value_node::set_id(node_id id)
	{
		id_ = id;
	}

	node_id value_node::get_id() const
	{
		return id_;
	}

}



