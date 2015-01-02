// reset_cmd_parser.h

#ifndef __WB_PARAMS_RESET_CMD_PARSER_H
#define __WB_PARAMS_RESET_CMD_PARSER_H

#include "nav_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	struct reset_cmd
	{
		boost::optional< nav_component > nav;
	};

	template < typename Iterator >
	struct reset_cmd_parser: qi::grammar< Iterator, reset_cmd(), qi::space_type >
	{
		reset_cmd_parser(bool enable_indexed);

		qi::rule< Iterator, reset_cmd(), qi::space_type > start;

		nav_parser< Iterator > unrestricted_nav;
	};

}


#endif


