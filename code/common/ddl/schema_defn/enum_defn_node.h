// enum_defn_node.h
/*
Options:
min/max selection
enumeration values
*/

#ifndef __WB_DDL_ENUM_DEFN_NODE_H
#define __WB_DDL_ENUM_DEFN_NODE_H

#include "defn_node_base_impl.h"

#include <boost/optional.hpp>

#include <string>
#include <vector>
#include <set>


namespace ddl {

	class navigator;

	class enum_defn_node:
		public detail::defn_node_base_impl
	{
	public:
		typedef std::string enum_value_t;
		// TODO: typedef std::set< enum_value_t > value_t;
		typedef std::vector< enum_value_t > value_t;
		typedef std::set< enum_value_t > enum_set_t;

		// TODO: Eventually may want to have named functions, with generic return type
		typedef std::function< enum_set_t(navigator) > enum_values_fn_t;

	public:
		enum_defn_node();

	public:
		void enumvalues_fn(enum_values_fn_t fn);
		enum_set_t enum_values(navigator nav) const;
		void minsel(size_t count);
		size_t minsel() const;
		void maxsel(boost::optional< size_t > count);
		boost::optional< size_t > maxsel() const;
		void default(value_t val);
		value_t default() const;

	private:
		enum_values_fn_t enum_values_fn_;
		size_t min_sel_;
		boost::optional< size_t > max_sel_;
		value_t default_;

		friend class defn_node;
	};

}


#endif


