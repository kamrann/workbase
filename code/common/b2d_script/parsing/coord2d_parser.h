// coord2d_parser.h

#ifndef __WB_B2D_SCRIPT_COORD_2D_PARSER_H
#define __WB_B2D_SCRIPT_COORD_2D_PARSER_H

#include "single_coord_parser.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		struct coord2d
		{
			single_coord x;
			single_coord y;
		};
	}

	template < typename Iterator >
	struct coord2d_parser: qi::grammar< Iterator, ast::coord2d(), qi::space_type >
	{
		coord2d_parser();

		qi::rule< Iterator, ast::coord2d(), qi::space_type > start_;

		single_coord_parser< Iterator > single_coord_parser_;
	};

}


#endif


