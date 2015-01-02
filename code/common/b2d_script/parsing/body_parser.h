// body_parser.h

#ifndef __WB_B2D_SCRIPT_BODY_PARSER_H
#define __WB_B2D_SCRIPT_BODY_PARSER_H

#include "name_parser.h"
#include "box_shape_parser.h"
#include "coord2d_parser.h"
#include "material_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		struct body
		{
			boost::optional< std::string > name;
			box_shape shape;	// todo: other types
			coord2d pos;
			boost::optional< material > mat;
		};
	}

	template < typename Iterator >
	struct body_parser: qi::grammar< Iterator, ast::body(), qi::space_type >
	{
		body_parser();

		qi::rule< Iterator, ast::body(), qi::space_type > start_;

		name_parser< Iterator > name_parser_;
		box_shape_parser< Iterator > box_shape_parser_;
		coord2d_parser< Iterator > coord2d_parser_;
		material_parser< Iterator > material_parser_;
	};

}


#endif


