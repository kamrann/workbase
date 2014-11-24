// defn_node.h

#ifndef __WB_DDL_DEFN_NODE_H
#define __WB_DDL_DEFN_NODE_H

#include "../ddl_types.h"
#include "defn_node_ptr.h"
#include "defn_node_type_from_impl.h"

#include <boost/variant.hpp>

#include <memory>


namespace ddl {

	namespace detail {

		typedef boost::variant <
			boolean_defn_node_ptr,
			integer_defn_node_ptr,
			realnum_defn_node_ptr,
			string_defn_node_ptr,
			enum_defn_node_ptr,
			list_defn_node_ptr,
			composite_defn_node_ptr,
			conditional_defn_node_ptr
		>
		defn_node_var_t;

	}


	class boolean_defn_node;
	class integer_defn_node;
	class realnum_defn_node;
	class string_defn_node;
	class enum_defn_node;
	class list_defn_node;
	class composite_defn_node;
	class conditional_defn_node;

	struct defn_holder;

	class defn_node
	{
	public:
		defn_node();
		defn_node(boolean_defn_node const& bn);
		defn_node(integer_defn_node const& in);
		defn_node(realnum_defn_node const& rn);
		defn_node(string_defn_node const& sn);
		defn_node(enum_defn_node const& en);
		defn_node(list_defn_node const& ln);
		defn_node(composite_defn_node const& cn);
		defn_node(conditional_defn_node const& cn);

		operator bool() const;

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
				typedef typename detail::defn_node_type_from_impl< NodeImpl >::type node_type;
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
			return boost::apply_visitor(node_vis, storage_->var_);
		}

	private:
//		detail::defn_node_var_t storage_;
		std::shared_ptr< defn_holder > storage_;
	};

	struct defn_holder
	{
		template < typename T >
		defn_holder(T const& v): var_{ v }
		{}

		node_id id_;
		// TODO: Think no longer any need for shared ptrs within the variant
		detail::defn_node_var_t var_;
	};

}


#endif


