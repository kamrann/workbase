// coord2d_parser.cpp

#include "coord2d_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::coord2d,
	(b2ds::ast::single_coord, x)
	(b2ds::ast::single_coord, y)
	);

namespace b2ds {

	template < typename Iterator >
	coord2d_parser< Iterator >::coord2d_parser():
		base_type(start_)
	{
		using qi::lit;

		start_ =
			lit(CoordOpen) >>
			single_coord_parser_ >>
			lit(',') >>
			single_coord_parser_ >>
			lit(CoordClose)
			;
	}

	template struct coord2d_parser < std::string::const_iterator > ;

}




