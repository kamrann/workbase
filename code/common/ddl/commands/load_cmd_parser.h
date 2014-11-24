// load_cmd_parser.h

#ifndef __WB_PARAMS_LOAD_CMD_PARSER_H
#define __WB_PARAMS_LOAD_CMD_PARSER_H

#include "nav_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	struct load_cmd
	{
		boost::optional< nav_component > nav;
		std::string filename;
	};

	template < typename Iterator >
	struct load_cmd_parser: qi::grammar< Iterator, load_cmd(), qi::space_type >
	{
		load_cmd_parser(bool enable_indexed);

		qi::rule< Iterator, std::string() > prefix;
		qi::rule< Iterator, std::string() > root;
		qi::rule< Iterator, std::string() > path_element;
		qi::rule< Iterator, std::string(), qi::space_type > filename;
		qi::rule< Iterator, load_cmd(), qi::space_type > start;

		nav_parser< Iterator > unrestricted_nav;
	};

}


#endif


