// enum_add_cmd_parser.h

#ifndef __WB_PARAMS_ENUM_ADD_CMD_PARSER_H
#define __WB_PARAMS_ENUM_ADD_CMD_PARSER_H

#include "../terminal_io/enum_element_parser.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	struct enum_add_cmd
	{
		std::vector< std::string > elements;
	};

	template < typename Iterator >
	struct enum_add_cmd_parser: qi::grammar< Iterator, enum_add_cmd(), qi::space_type >
	{
		enum_add_cmd_parser();

		qi::rule< Iterator, enum_add_cmd(), qi::space_type > start;

		enum_element_parser< Iterator > element;
	};

}


#endif


