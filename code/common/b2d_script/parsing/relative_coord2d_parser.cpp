// relative_coord2d_parser.cpp

#include "relative_coord2d_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::rel_coord2d,
	(std::string, ref)
	(b2ds::ast::coord2d, crd)
	);

namespace b2ds {

	template < typename Iterator >
	relative_coord2d_parser< Iterator >::relative_coord2d_parser():
		base_type(start_)
	{
		using qi::lit;

		start_ =
			name_parser_ >>
			lit('@') >>
			coord2d_parser_
			;
	}

	template struct relative_coord2d_parser < std::string::const_iterator > ;

}




