// single_coord_parser.cpp

#include "single_coord_parser.h"


namespace b2ds {

	template < typename Iterator >
	single_coord_parser< Iterator >::single_coord_parser():
		base_type(start_)
	{
		using qi::int_;

		start_ =
			int_
			;
	}

	template struct single_coord_parser < std::string::const_iterator > ;

}




