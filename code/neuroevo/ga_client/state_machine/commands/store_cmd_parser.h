// store_cmd_parser.h

#ifndef __WB_GA_CLIENT_STORE_CMD_PARSER_H
#define __WB_GA_CLIENT_STORE_CMD_PARSER_H

#include "params/param.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace ga_control {

	struct store_cmd
	{
		size_t index;
		boost::optional< std::string > name;
	};

	template < typename Iterator >
	struct store_cmd_parser: qi::grammar< Iterator, store_cmd(), qi::space_type >
	{
		store_cmd_parser();

		qi::rule< Iterator, std::string() > name_;
		qi::rule< Iterator, store_cmd(), qi::space_type > start;
	};

}


#endif


