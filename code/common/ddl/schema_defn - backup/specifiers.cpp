// specifiers.cpp

#include "specifiers.h"

#include "boolean_defn_node.h"
#include "integer_defn_node.h"
#include "realnum_defn_node.h"
#include "string_defn_node.h"
#include "enum_defn_node.h"
#include "list_defn_node.h"
#include "composite_defn_node.h"
#include "conditional_defn_node.h"


namespace ddl {

	specifier::specifier():
		next_id_{ 0 }
	{

	}

	boolean_defn_node specifier::boolean()
	{
		return boolean_defn_node{};
	}

	integer_defn_node specifier::integer()
	{
		return integer_defn_node{};
	}

	realnum_defn_node specifier::realnum()
	{
		return realnum_defn_node{};
	}

	string_defn_node specifier::string()
	{
		return string_defn_node{};
	}

	enum_defn_node specifier::enumeration()
	{
		return enum_defn_node{};
	}

	list_defn_node specifier::list()
	{
		return list_defn_node{};
	}

	composite_defn_node specifier::composite()
	{
		return composite_defn_node{};
	}

	conditional_defn_node specifier::conditional()
	{
		return conditional_defn_node{};
	}

}




