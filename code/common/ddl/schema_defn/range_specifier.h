// range_specifier.h

#ifndef __WB_DDL_RANGE_SPECIFIER_H
#define __WB_DDL_RANGE_SPECIFIER_H

#include <boost/optional.hpp>


namespace ddl {

	template < typename T >
	struct spc_range
	{
		spc_range(boost::optional< T > min, boost::optional< T > max):
			min_{ std::move(min) },
			max_{ std::move(max) }
		{}

		boost::optional< T > min_, max_;
	};

}


#endif

