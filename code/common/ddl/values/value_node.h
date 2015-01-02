// value_node.h

#ifndef __WB_DDL_VALUE_NODE_H
#define __WB_DDL_VALUE_NODE_H

#include "../schema_inst/boolean_sch_node.h"
#include "../schema_inst/integer_sch_node.h"
#include "../schema_inst/realnum_sch_node.h"
#include "../schema_inst/string_sch_node.h"
#include "../schema_inst/enum_sch_node.h"
#include "../schema_inst/list_sch_node.h"
#include "../schema_inst/composite_sch_node.h"
#include "../schema_inst/conditional_sch_node.h"

#include "value.h"

#include <boost/variant.hpp>


namespace ddl {

	struct value_holder;

	class value_node
	{
	public:
		value_node();

		bool initialized() const;
		operator bool() const;

	public:
		value_node& operator= (boolean_sch_node::value_t const& val);
		value_node& operator= (integer_sch_node::value_t const& val);
		value_node& operator= (realnum_sch_node::value_t const& val);
		value_node& operator= (string_sch_node::value_t const& val);
		value_node& operator= (enum_sch_node::value_t const& val);
		value_node& operator= (enum_sch_node::str_value_t const& val);
		value_node& operator= (std::vector< value_node > const& val);
		value_node& operator= (std::map< node_name, value_node > const& val);

		value_node& operator= (value const& val);

		void assign(value_node rhs);

	public:
		bool is_bool() const;
		bool is_int() const;
		bool is_real() const;
		bool is_string() const;
		bool is_enum() const;
		bool is_list() const;
		bool is_composite() const;
		bool is_leaf() const;

		boolean_sch_node::value_t as_bool() const;
		integer_sch_node::value_t as_int() const;
		realnum_sch_node::value_t as_real() const;
		string_sch_node::value_t as_string() const;
		enum_sch_node::value_t as_enum() const;
		std::vector< std::string > as_enum_str() const;
		std::vector< value_node > as_list() const;
		std::map< node_name, value_node > as_composite() const;

		value as_value() const;

		// Shortcut access
		enum_sch_node::enum_value_t as_single_enum() const;
		std::string as_single_enum_str() const;

		template < typename T >
		T as_single_enum_data() const
		{
			return boost::any_cast<T>(as_single_enum().data);
		}

		void set_id(node_id id);
		node_id get_id() const;

	private:
		/*		template < typename Vis >
		struct node_visitor:
		public boost::static_visitor < typename Vis::result_type >
		{
		node_visitor(Vis& vis):
		vis_{ vis }
		{}

		template < typename ValueType >
		inline result_type operator() (ValueType const& val)
		{
		return vis_(n);
		}

		Vis& vis_;
		};
		*/
	public:
		template < typename Visitor >
		auto apply_visitor(Visitor& vis) const -> typename Visitor::result_type
		{
			//			auto node_vis = node_visitor < Visitor > { vis };
			return boost::apply_visitor(vis/*node_vis*/, *storage_);
		}

	private:
		std::shared_ptr< value_holder > storage_;
		node_id id_;
	};

	typedef boost::variant<
		boolean_sch_node::value_t,
		integer_sch_node::value_t,
		realnum_sch_node::value_t,
		string_sch_node::value_t,
		enum_sch_node::value_t,
		std::vector< value_node >,
		std::map< node_name, value_node >
	>
	value_sch_node_var_t;

	struct value_holder:
		public value_sch_node_var_t
	{
		template < typename T >
		value_holder& operator= (T const& rhs)
		{
			value_sch_node_var_t::operator= (rhs);
			return *this;
		}
	};

}


#endif


