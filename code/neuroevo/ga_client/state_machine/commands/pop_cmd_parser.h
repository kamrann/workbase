// pop_cmd_parser.h

#ifndef __WB_GA_CLIENT_POP_CMD_PARSER_H
#define __WB_GA_CLIENT_POP_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace ga_control {

	struct pop_cmd
	{

	};

	template < typename Iterator >
	struct pop_cmd_parser: qi::grammar< Iterator, pop_cmd(), qi::space_type >
	{
		pop_cmd_parser();

		qi::rule< Iterator, pop_cmd(), qi::space_type > start;
	};

}


#endif


