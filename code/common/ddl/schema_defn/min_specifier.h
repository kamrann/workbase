// min_specifier.h

#ifndef __WB_DDL_MIN_SPECIFIER_H
#define __WB_DDL_MIN_SPECIFIER_H

#include "basic_value_specifier.h"


namespace ddl {

	template < typename T >
	struct spc_min: public spc_basic_value < T >
	{
		using spc_basic_value< T >::spc_basic_value;
	};

}


#endif

