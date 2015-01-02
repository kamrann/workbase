// box_shape_parser.h

#ifndef __WB_B2D_SCRIPT_BOX_SHAPE_PARSER_H
#define __WB_B2D_SCRIPT_BOX_SHAPE_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		struct box_shape
		{
			unsigned int width;
			unsigned int height;
		};
	}

	template < typename Iterator >
	struct box_shape_parser: qi::grammar< Iterator, ast::box_shape(), qi::space_type >
	{
		box_shape_parser();

		qi::rule< Iterator, ast::box_shape(), qi::space_type > start_;
	};

}


#endif


