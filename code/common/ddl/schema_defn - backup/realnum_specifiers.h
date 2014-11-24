// realnum_specifiers.h

#ifndef __WB_DDL_REALNUM_SPECIFIERS_H
#define __WB_DDL_REALNUM_SPECIFIERS_H

#include "realnum_defn_node.h"


namespace ddl {

	class define_real_fixed
	{
	public:
		define_real_fixed(realnum_defn_node::value_t dft):
			default_{ dft }
		{}

	public:
		operator realnum_defn_node() const
		{
			realnum_defn_node nd;
			nd.default(default_);
			return nd;
		}

	private:
		realnum_defn_node::value_t default_;
	};

}


#endif

