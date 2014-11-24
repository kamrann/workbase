// defn_node_ptr.h

#ifndef __WB_DDL_DEFN_NODE_PTR_H
#define __WB_DDL_DEFN_NODE_PTR_H

#include <memory>


namespace ddl {
	namespace detail {

		class boolean_defn_node_t;
		class integer_defn_node_t;
		class realnum_defn_node_t;
		class string_defn_node_t;
		class enum_defn_node_t;
		class list_defn_node_t;
		class composite_defn_node_t;
		class conditional_defn_node_t;

		typedef std::shared_ptr< boolean_defn_node_t > boolean_defn_node_ptr;
		typedef std::shared_ptr< integer_defn_node_t > integer_defn_node_ptr;
		typedef std::shared_ptr< realnum_defn_node_t > realnum_defn_node_ptr;
		typedef std::shared_ptr< string_defn_node_t > string_defn_node_ptr;
		typedef std::shared_ptr< enum_defn_node_t > enum_defn_node_ptr;
		typedef std::shared_ptr< list_defn_node_t > list_defn_node_ptr;
		typedef std::shared_ptr< composite_defn_node_t > composite_defn_node_ptr;
		typedef std::shared_ptr< conditional_defn_node_t > conditional_defn_node_ptr;

	}
}


#endif


