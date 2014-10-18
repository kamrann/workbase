// debug_cmd_parser.h

#ifndef __WB_PARAMS_DEBUG_CMD_PARSER_H
#define __WB_PARAMS_DEBUG_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	struct debug_cmd
	{

	};

	template < typename Iterator >
	struct debug_cmd_parser: qi::grammar< Iterator, debug_cmd(), qi::space_type >
	{
		debug_cmd_parser();

		qi::rule< Iterator, debug_cmd(), qi::space_type > start;
	};

}


#endif


