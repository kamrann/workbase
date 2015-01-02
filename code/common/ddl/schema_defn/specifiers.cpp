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

	boolean_specifier specifier::boolean(std::string name)
	{
		return id_assigned(boolean_defn_node{}, name);
	}

	integer_specifier specifier::integer(std::string name)
	{
		return id_assigned(integer_defn_node{}, name);
	}

	realnum_specifier specifier::realnum(std::string name)
	{
		return id_assigned(realnum_defn_node{}, name);
	}

	string_specifier specifier::string(std::string name)
	{
		return id_assigned(string_defn_node{}, name);
	}

	enum_specifier specifier::enumeration(std::string name)
	{
		return id_assigned(enum_defn_node{}, name);
	}

	list_specifier specifier::list(std::string name)
	{
		return id_assigned(list_defn_node{}, name);
	}

	composite_specifier specifier::composite(std::string name)
	{
		return id_assigned(composite_defn_node{}, name);
	}

	conditional_specifier specifier::conditional(std::string name)
	{
		return id_assigned(conditional_defn_node{}, name);
	}

}




