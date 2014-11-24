// basic_value_specifier.h

#ifndef __WB_DDL_BASIC_VALUE_SPECIFIER_H
#define __WB_DDL_BASIC_VALUE_SPECIFIER_H


namespace ddl {

	template < typename T >
	struct spc_basic_value
	{
		spc_basic_value(T v): val_{ std::move(v) }
		{}

		T val_;
	};

}


#endif

