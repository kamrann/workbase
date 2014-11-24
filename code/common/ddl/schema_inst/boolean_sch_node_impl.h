// boolean_sch_node_impl.h

#ifndef __WB_DDL_BOOLEAN_SCHEMA_NODE_IMPL_H
#define __WB_DDL_BOOLEAN_SCHEMA_NODE_IMPL_H


namespace ddl {

	namespace detail {

		class boolean_sch_node_t
		{
		public:
			typedef bool value_t;

		public:
			boolean_sch_node_t();

		public:
			value_t default_;
		};

	}

}


#endif

