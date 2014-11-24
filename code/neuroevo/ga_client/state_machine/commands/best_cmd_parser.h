// best_cmd_parser.h

#ifndef __WB_GA_CLIENT_BEST_CMD_PARSER_H
#define __WB_GA_CLIENT_BEST_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace ga_control {

	struct best_cmd
	{

	};

	template < typename Iterator >
	struct best_cmd_parser: qi::grammar< Iterator, best_cmd(), qi::space_type >
	{
		best_cmd_parser();

		qi::rule< Iterator, best_cmd(), qi::space_type > start;
	};

}


#endif


