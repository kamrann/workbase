// single_coord_parser.h

#ifndef __WB_B2D_SCRIPT_SINGLE_COORD_PARSER_H
#define __WB_B2D_SCRIPT_SINGLE_COORD_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		typedef int single_coord;
	}

	template < typename Iterator >
	struct single_coord_parser: qi::grammar< Iterator, ast::single_coord(), qi::space_type >
	{
		single_coord_parser();

		qi::rule< Iterator, ast::single_coord(), qi::space_type > start_;
	};

}


#endif


