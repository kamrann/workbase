// enum_defn_node.h
/*
Options:
min/max selection
enumeration values
*/

#ifndef __WB_DDL_ENUM_DEFN_NODE_H
#define __WB_DDL_ENUM_DEFN_NODE_H

#include "defn_node_base_impl.h"
#include "../dependencies.h"

#include <boost/optional.hpp>
#include <boost/any.hpp>

#include <string>
#include <vector>
#include <set>


namespace ddl {

	class navigator;

	class enum_defn_node:
		public detail::defn_node_base_impl
	{
	public:
		//typedef std::string enum_value_t;
		//typedef std::vector< enum_value_t > value_t;
		//typedef std::set< enum_value_t > enum_set_t;

		struct enum_value_t
		{
			std::string str;
			boost::any data;
		};

		// TODO: ideally this would be a set, and would somehow reference an element of the enum_set_t
		// in such a way as to prevent the same enum value being included twice.
		// perhaps a set of integer indices into the enum_set_t vector, and then the schema node would be
		// accessed to retrieve the actual string/data pair.
		typedef std::vector< enum_value_t > value_t;	// Specifies selection (ie. the value type of the node instance)
		typedef std::vector< std::string > str_value_t;

		typedef std::vector< enum_value_t > enum_set_t;	// Specified all possible values
		typedef std::vector< std::string > enum_str_set_t;	// Specified all possible values

		// TODO: Eventually may want to have named functions
		typedef dep_function< enum_set_t > enum_values_fn_t;
		typedef dep_function< std::vector< std::string > > enum_values_str_fn_t;

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

		dep_list deps() const;

	private:
		enum_values_fn_t enum_values_fn_;
		size_t min_sel_;
		boost::optional< size_t > max_sel_;
		value_t default_;

		friend class defn_node;
	};

}


#endif


