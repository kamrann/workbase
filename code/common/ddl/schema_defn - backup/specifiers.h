// specifiers.h

#ifndef __WB_DDL_SPECIFIERS_H
#define __WB_DDL_SPECIFIERS_H

#include "defn_node.h"
/*
#include "boolean_specifiers.h"
#include "integer_specifiers.h"
#include "realnum_specifiers.h"
#include "string_specifiers.h"
#include "enum_specifiers.h"
#include "list_specifiers.h"
#include "composite_specifiers.h"
*/

namespace ddl {

	class specifier
	{
	public:
		specifier();

	public:
		boolean_defn_node boolean();
		integer_defn_node integer();
		realnum_defn_node realnum();
		string_defn_node string();
		enum_defn_node enumeration();
		list_defn_node list();
		composite_defn_node composite();
		conditional_defn_node conditional();

	private:


	private:
		unsigned int next_id_;
	};

}


#endif

