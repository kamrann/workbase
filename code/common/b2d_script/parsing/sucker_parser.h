// sucker_parser.h

#ifndef __B2D_SCRIPT_SUCKER_PARSER_H
#define __B2D_SCRIPT_SUCKER_PARSER_H

#include "name_parser.h"
#include "relative_coord2d_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		struct sucker
		{
			boost::optional< std::string > name;
			rel_coord2d location;
		};
	}
	

	template < typename Iterator >
	struct sucker_parser: qi::grammar< Iterator, ast::sucker(), qi::space_type >
	{
		sucker_parser();

		qi::rule< Iterator, ast::sucker(), qi::space_type > start_;

		name_parser< Iterator > name_parser_;
		relative_coord2d_parser< Iterator > rel_coord_parser_;
	};

}


#endif


