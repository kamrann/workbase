// enum_remove_cmd_parser.h

#ifndef __WB_PARAMS_ENUM_REMOVE_CMD_PARSER_H
#define __WB_PARAMS_ENUM_REMOVE_CMD_PARSER_H

#include "enum_element_parser.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	struct enum_remove_cmd
	{
		std::vector< std::string > elements;
	};

	template < typename Iterator >
	struct enum_remove_cmd_parser: qi::grammar< Iterator, enum_remove_cmd(), qi::space_type >
	{
		enum_remove_cmd_parser();

		qi::rule< Iterator, enum_remove_cmd(), qi::space_type > start;

		enum_element_parser< Iterator > element;
	};

}


#endif


