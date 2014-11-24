// repeat_add_cmd_parser.h

#ifndef __WB_PARAMS_REPEAT_ADD_CMD_PARSER_H
#define __WB_PARAMS_REPEAT_ADD_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	struct repeat_add_cmd
	{};

	template < typename Iterator >
	struct repeat_add_cmd_parser: qi::grammar< Iterator, repeat_add_cmd(), qi::space_type >
	{
		repeat_add_cmd_parser();

		qi::rule< Iterator, repeat_add_cmd(), qi::space_type > start;
	};

}


#endif


