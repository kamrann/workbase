// get_cmd_parser.h

#ifndef __WB_SYSSIM_CLIENT_GET_CMD_PARSER_H
#define __WB_SYSSIM_CLIENT_GET_CMD_PARSER_H

#include "ddl/terminal_io/enum_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace sys_control {

	struct get_cmd
	{
		ddl::enum_param_val values;
	};

	template < typename Iterator >
	struct get_cmd_parser: qi::grammar< Iterator, get_cmd(), qi::space_type >
	{
		get_cmd_parser();

		qi::rule< Iterator, get_cmd(), qi::space_type > start;

		typename ddl::enum_parser< Iterator >::lax enum_vals;
	};

}


#endif


