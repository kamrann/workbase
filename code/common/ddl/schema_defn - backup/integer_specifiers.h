// integer_specifiers.h

#ifndef __WB_DDL_INTEGER_SPECIFIERS_H
#define __WB_DDL_INTEGER_SPECIFIERS_H

#include "integer_defn_node.h"


namespace ddl {

	class define_integer_fixed
	{
	public:
		define_integer_fixed(integer_defn_node::value_t dft):
			default_{ dft }
		{}

	public:
		operator integer_defn_node() const
		{
			integer_defn_node nd;
			nd.default(default_);
			return nd;
		}

	private:
		integer_defn_node::value_t default_;
	};

}


#endif

