// sch_node_ptr.h

#ifndef __WB_DDL_SCHEMA_NODE_PTR_H
#define __WB_DDL_SCHEMA_NODE_PTR_H

#include <memory>


namespace ddl {
	namespace detail {

		class boolean_sch_node_t;
		class integer_sch_node_t;
		class realnum_sch_node_t;
		class string_sch_node_t;
		class enum_sch_node_t;
		class list_sch_node_t;
		class composite_sch_node_t;

		typedef std::shared_ptr< boolean_sch_node_t > boolean_sch_node_ptr;
		typedef std::shared_ptr< integer_sch_node_t > integer_sch_node_ptr;
		typedef std::shared_ptr< realnum_sch_node_t > realnum_sch_node_ptr;
		typedef std::shared_ptr< string_sch_node_t > string_sch_node_ptr;
		typedef std::shared_ptr< enum_sch_node_t > enum_sch_node_ptr;
		typedef std::shared_ptr< list_sch_node_t > list_sch_node_ptr;
		typedef std::shared_ptr< composite_sch_node_t > composite_sch_node_ptr;

	}
}


#endif


