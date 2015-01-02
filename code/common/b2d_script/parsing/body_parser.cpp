// body_parser.cpp

#include "body_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::body,
	(boost::optional< std::string >, name)
	(b2ds::ast::box_shape, shape)
	(b2ds::ast::coord2d, pos)
	(boost::optional< b2ds::ast::material >, mat)
	);

namespace b2ds {

	template < typename Iterator >
	body_parser< Iterator >::body_parser():
		base_type(start_)
	{
		using qi::lit;

		start_ =
			lit("body") >>
			-(lit(':') >> name_parser_) >>
			lit(ArgListOpen) >>
			box_shape_parser_ >>
			lit("@") >>
			coord2d_parser_ >>
			// TODO: permutation rather than sequence?
			-material_parser_ >>
			lit(ArgListClose);
			;
	}

	template struct body_parser < std::string::const_iterator > ;

}




