// sch_node_type_from_impl.h

#ifndef __WB_DDL_SCHEMA_NODE_TYPE_FROM_IMPL_H
#define __WB_DDL_SCHEMA_NODE_TYPE_FROM_IMPL_H


namespace ddl {

	class boolean_sch_node;
	class integer_sch_node;
	class realnum_sch_node;
	class string_sch_node;
	class enum_sch_node;
	class list_sch_node;
	class composite_sch_node;

	namespace detail {

		class boolean_sch_node_t;
		class integer_sch_node_t;
		class realnum_sch_node_t;
		class string_sch_node_t;
		class enum_sch_node_t;
		class list_sch_node_t;
		class composite_sch_node_t;

		template < typename NodeImpl >
		struct sch_node_type_from_impl;

		template <>
		struct sch_node_type_from_impl< boolean_sch_node_t >
		{
			typedef boolean_sch_node type;
		};

		template <>
		struct sch_node_type_from_impl< integer_sch_node_t >
		{
			typedef integer_sch_node type;
		};

		template <>
		struct sch_node_type_from_impl< realnum_sch_node_t >
		{
			typedef realnum_sch_node type;
		};

		template <>
		struct sch_node_type_from_impl< string_sch_node_t >
		{
			typedef string_sch_node type;
		};

		template <>
		struct sch_node_type_from_impl< enum_sch_node_t >
		{
			typedef enum_sch_node type;
		};

		template <>
		struct sch_node_type_from_impl< list_sch_node_t >
		{
			typedef list_sch_node type;
		};

		template <>
		struct sch_node_type_from_impl< composite_sch_node_t >
		{
			typedef composite_sch_node type;
		};

	}
}


#endif


