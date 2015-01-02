// enum_sch_node.h
/*
Options:
min/max selection
enumeration values
*/

#ifndef __WB_DDL_ENUM_SCHEMA_NODE_H
#define __WB_DDL_ENUM_SCHEMA_NODE_H

#include "enum_sch_node_impl.h"
#include "sch_node_ptr.h"

#include <boost/optional.hpp>

#include <string>
#include <vector>
#include <set>
#include <memory>


namespace ddl {

	class enum_sch_node
	{
	public:
		typedef detail::enum_sch_node_t impl_t;
		typedef impl_t::enum_value_t enum_value_t;
		typedef impl_t::value_t value_t;
		typedef impl_t::str_value_t str_value_t;
		typedef impl_t::enum_set_t enum_set_t;

	public:
		enum_sch_node();
		enum_sch_node(detail::enum_sch_node_ptr ptr);

	public:
//		void add_option(enum_value_t val);
		void enum_values(enum_set_t values);
		enum_set_t enum_values() const;
		std::vector< std::string > enum_values_str() const;
		boost::any get_data_from_id_string(std::string str) const;
		void minsel(size_t count);
		size_t minsel() const;
		void maxsel(boost::optional< size_t > count);
		boost::optional< size_t > maxsel() const;
		void default(value_t val);
		value_t default() const;

	private:
		detail::enum_sch_node_ptr ptr() const
		{
			return obj_;
		}

	private:
		std::shared_ptr< impl_t > obj_;

		friend class sch_node;
	};

}


#endif


