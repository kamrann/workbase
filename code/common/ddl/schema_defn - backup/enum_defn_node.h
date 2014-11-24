// enum_defn_node.h
/*
Options:
min/max selection
enumeration values
*/

#ifndef __WB_DDL_ENUM_DEFN_NODE_H
#define __WB_DDL_ENUM_DEFN_NODE_H

#include "enum_defn_node_impl.h"
#include "defn_node_ptr.h"

#include <boost/optional.hpp>

#include <string>
#include <vector>
#include <set>
#include <memory>


namespace ddl {

	class enum_defn_node
	{
	public:
		typedef detail::enum_defn_node_t impl_t;
		typedef impl_t::enum_value_t enum_value_t;
		typedef impl_t::value_t value_t;
		typedef impl_t::enum_set_t enum_set_t;
		typedef impl_t::enum_values_fn_t enum_values_fn_t;

	public:
		enum_defn_node();
		enum_defn_node(detail::enum_defn_node_ptr ptr);

	public:
		void enumvalues_fn(enum_values_fn_t fn);
		enum_set_t enum_values() const;
		void minsel(size_t count);
		size_t minsel() const;
		void maxsel(boost::optional< size_t > count);
		boost::optional< size_t > maxsel() const;

	private:
		detail::enum_defn_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class defn_node;
	};

}


#endif


