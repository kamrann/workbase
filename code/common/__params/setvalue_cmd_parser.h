// setvalue_cmd_parser.h

#ifndef __WB_PARAMS_SETVALUE_CMD_PARSER_H
#define __WB_PARAMS_SETVALUE_CMD_PARSER_H

#include "nav_parser.h"
#include "value_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	struct setvalue_cmd
	{
		boost::optional< nav_component > nav;
		param val;
	};

	template < typename Iterator >
	struct setvalue_cmd_parser: qi::grammar< Iterator, setvalue_cmd(), qi::space_type >
	{
		setvalue_cmd_parser(bool enable_indexed, bool require_nav);

		qi::rule< Iterator, boost::optional< nav_component >(), qi::space_type > nav;
		qi::rule< Iterator, setvalue_cmd(), qi::space_type > start;

		nav_parser< Iterator > unrestricted_nav;
		value_parser< Iterator > param_value_parser;
	};

}


#endif


