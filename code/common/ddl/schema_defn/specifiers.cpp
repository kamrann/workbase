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

	boolean_specifier specifier::boolean()
	{
		return id_assigned(boolean_defn_node{});
	}

	integer_specifier specifier::integer()
	{
		return id_assigned(integer_defn_node{});
	}

	realnum_specifier specifier::realnum()
	{
		return id_assigned(realnum_defn_node{});
	}

	string_specifier specifier::string()
	{
		return id_assigned(string_defn_node{});
	}

	enum_specifier specifier::enumeration()
	{
		return id_assigned(enum_defn_node{});
	}

	list_specifier specifier::list()
	{
		return id_assigned(list_defn_node{});
	}

	composite_specifier specifier::composite()
	{
		return id_assigned(composite_defn_node{});
	}

	conditional_specifier specifier::conditional()
	{
		return id_assigned(conditional_defn_node{});
	}

}




