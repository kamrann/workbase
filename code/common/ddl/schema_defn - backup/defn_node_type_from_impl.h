// defn_node_type_from_impl.h

#ifndef __WB_DDL_DEFN_NODE_TYPE_FROM_IMPL_H
#define __WB_DDL_DEFN_NODE_TYPE_FROM_IMPL_H


namespace ddl {

	class boolean_defn_node;
	class integer_defn_node;
	class realnum_defn_node;
	class string_defn_node;
	class enum_defn_node;
	class list_defn_node;
	class composite_defn_node;
	class conditional_defn_node;

	namespace detail {

		class boolean_defn_node_t;
		class integer_defn_node_t;
		class realnum_defn_node_t;
		class string_defn_node_t;
		class enum_defn_node_t;
		class list_defn_node_t;
		class composite_defn_node_t;
		class conditional_defn_node_t;

		template < typename NodeImpl >
		struct defn_node_type_from_impl;

		template <>
		struct defn_node_type_from_impl< boolean_defn_node_t >
		{
			typedef boolean_defn_node type;
		};

		template <>
		struct defn_node_type_from_impl< integer_defn_node_t >
		{
			typedef integer_defn_node type;
		};

		template <>
		struct defn_node_type_from_impl< realnum_defn_node_t >
		{
			typedef realnum_defn_node type;
		};

		template <>
		struct defn_node_type_from_impl< string_defn_node_t >
		{
			typedef string_defn_node type;
		};

		template <>
		struct defn_node_type_from_impl< enum_defn_node_t >
		{
			typedef enum_defn_node type;
		};

		template <>
		struct defn_node_type_from_impl< list_defn_node_t >
		{
			typedef list_defn_node type;
		};

		template <>
		struct defn_node_type_from_impl< composite_defn_node_t >
		{
			typedef composite_defn_node type;
		};

		template <>
		struct defn_node_type_from_impl< conditional_defn_node_t >
		{
			typedef conditional_defn_node type;
		};

	}
}


#endif


