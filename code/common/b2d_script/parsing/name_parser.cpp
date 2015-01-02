// name_parser.cpp

#include "name_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


namespace b2ds {

	template < typename Iterator >
	name_parser< Iterator >::name_parser():
		base_type(start_)
	{
		using qi::char_;
		using qi::alpha;

		// TODO:
		start_ =
			*(alpha | char_('_'))
			;
	}

	template struct name_parser < std::string::const_iterator > ;

}




