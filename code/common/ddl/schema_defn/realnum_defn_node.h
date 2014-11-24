// realnum_defn_node.h
/*
Options:
range
default
*/

#ifndef __WB_DDL_REALNUM_DEFN_NODE_H
#define __WB_DDL_REALNUM_DEFN_NODE_H

#include "defn_node_base_impl.h"

#include <boost/optional.hpp>


namespace ddl {

	class realnum_defn_node:
		public detail::defn_node_base_impl
	{
	public:
		typedef double value_t;

	public:
		realnum_defn_node();

	public:
		void default(value_t val);
		value_t default() const;
		void min(boost::optional< value_t > val);
		boost::optional< value_t > min() const;
		void max(boost::optional< value_t > val);
		boost::optional< value_t > max() const;

	private:
		boost::optional< value_t > min_, max_;
		value_t default_;

		friend class defn_node;
	};

}


#endif


