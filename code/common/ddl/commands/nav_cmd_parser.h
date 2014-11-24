// nav_cmd_parser.h

#ifndef __WB_PARAMS_NAV_CMD_PARSER_H
#define __WB_PARAMS_NAV_CMD_PARSER_H

#include "nav_parser.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	typedef nav_component nav_cmd;

	template < typename Iterator >
	struct nav_cmd_parser: qi::grammar< Iterator, nav_cmd(), qi::space_type >
	{
		nav_cmd_parser(bool enable_indexed);

		qi::rule< Iterator, nav_cmd(), qi::space_type > start;

		nav_parser< Iterator > restricted_nav;
		nav_parser< Iterator > unrestricted_nav;
	};

}


#endif


