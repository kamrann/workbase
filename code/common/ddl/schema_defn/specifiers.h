// specifiers.h

#ifndef __WB_DDL_SPECIFIERS_H
#define __WB_DDL_SPECIFIERS_H

#include "defn_node.h"

#include "boolean_specifiers.h"
#include "integer_specifiers.h"
#include "realnum_specifiers.h"
#include "string_specifiers.h"
#include "enum_specifiers.h"
#include "list_specifiers.h"
#include "composite_specifiers.h"
#include "conditional_specifiers.h"

#include <boost/optional.hpp>


namespace ddl {

	class specifier
	{
	public:
		specifier();

	public:
		boolean_specifier boolean(std::string name);
		integer_specifier integer(std::string name);
		realnum_specifier realnum(std::string name);
		string_specifier string(std::string name);
		enum_specifier enumeration(std::string name);
		list_specifier list(std::string name);
		composite_specifier composite(std::string name);
		conditional_specifier conditional(std::string name);

	private:
		template < typename T >
		T id_assigned(T&& n, std::string name)
		{
			n.set_id(next_id_++);
			n.set_name(name);
			return n;
		}

	private:
		unsigned int next_id_;
	};

}


#endif

