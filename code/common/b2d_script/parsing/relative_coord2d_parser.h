// relative_coord2d_parser.h

#ifndef __WB_B2D_SCRIPT_RELATIVE_COORD_2D_PARSER_H
#define __WB_B2D_SCRIPT_RELATIVE_COORD_2D_PARSER_H

#include "name_parser.h"
#include "coord2d_parser.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		struct rel_coord2d
		{
			std::string ref;
			coord2d crd;
		};
	}

	template < typename Iterator >
	struct relative_coord2d_parser: qi::grammar< Iterator, ast::rel_coord2d(), qi::space_type >
	{
		relative_coord2d_parser();

		qi::rule< Iterator, ast::rel_coord2d(), qi::space_type > start_;

		name_parser< Iterator > name_parser_;
		coord2d_parser< Iterator > coord2d_parser_;
	};

}


#endif


