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
		boolean_specifier boolean();
		integer_specifier integer();
		realnum_specifier realnum();
		string_specifier string();
		enum_specifier enumeration();
		list_specifier list();
		composite_specifier composite();
		conditional_specifier conditional();

	private:
		template < typename T >
		T id_assigned(T&& n)
		{
			n.set_id(next_id_++);
			return n;
		}

	private:
		unsigned int next_id_;
	};

}


#endif

