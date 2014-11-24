// sch_node.h

#ifndef __WB_DDL_SCHEMA_NODE_H
#define __WB_DDL_SCHEMA_NODE_H

#include "sch_node_ptr.h"
#include "sch_node_type_from_impl.h"
#include "../ddl_types.h"

#include <boost/variant.hpp>

#include <memory>


namespace ddl {

	namespace detail {

		typedef boost::variant <
			boolean_sch_node_ptr,
			integer_sch_node_ptr,
			realnum_sch_node_ptr,
			string_sch_node_ptr,
			enum_sch_node_ptr,
			list_sch_node_ptr,
			composite_sch_node_ptr
		>
		sch_node_var_t;

	}


	class boolean_sch_node;
	class integer_sch_node;
	class realnum_sch_node;
	class string_sch_node;
	class enum_sch_node;
	class list_sch_node;
	class composite_sch_node;

	class sch_node
	{
	public:
		sch_node();
		sch_node(boolean_sch_node const& bn);
		sch_node(integer_sch_node const& in);
		sch_node(realnum_sch_node const& rn);
		sch_node(string_sch_node const& sn);
		sch_node(enum_sch_node const& en);
		sch_node(list_sch_node const& ln);
		sch_node(composite_sch_node const& cn);

		operator bool() const;

	public:
		void set_id(node_id id);
		node_id get_id() const;

	private:
		template < typename Vis >
		struct node_visitor:
			public boost::static_visitor < typename Vis::result_type >
		{
			node_visitor(Vis& vis):
				vis_{ vis }
			{}

			template < typename NodeImpl >
			inline result_type operator() (std::shared_ptr< NodeImpl > const& ptr)// const
			{
				typedef typename detail::sch_node_type_from_impl< NodeImpl >::type node_type;
				node_type n{ ptr };
				return vis_(n);
			}

			Vis& vis_;
		};

	public:
		template < typename Visitor >
		auto apply_visitor(Visitor& vis) const -> typename Visitor::result_type
		{
			auto node_vis = node_visitor < Visitor > { vis };
			return boost::apply_visitor(node_vis, storage_);
		}

	private:
		detail::sch_node_var_t storage_;
		// TODO: should id be part of storage???
		node_id id_;
	};

}


#endif


