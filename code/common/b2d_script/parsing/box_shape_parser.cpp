// box_shape_parser.cpp

#include "box_shape_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::box_shape,
	(unsigned int, width)
	(unsigned int, height)
	);

namespace b2ds {

	template < typename Iterator >
	box_shape_parser< Iterator >::box_shape_parser():
		base_type(start_)
	{
		using qi::lit;
		using qi::uint_;

		start_ =
			lit("box") >>
			lit(ArgListOpen) >>
			uint_ >>
			(lit('x') | lit('*')) >>
			uint_ >>
			lit(ArgListClose)
			;
	}

	template struct box_shape_parser < std::string::const_iterator > ;

}




