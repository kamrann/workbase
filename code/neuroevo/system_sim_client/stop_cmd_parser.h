// stop_cmd_parser.h

#ifndef __WB_SYSSIM_CLIENT_STOP_CMD_PARSER_H
#define __WB_SYSSIM_CLIENT_STOP_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace sys_control {

	struct stop_cmd
	{
// TODO:		boost::optional< unsigned int > frames;
	};

	template < typename Iterator >
	struct stop_cmd_parser: qi::grammar< Iterator, stop_cmd(), qi::space_type >
	{
		stop_cmd_parser();

		qi::rule< Iterator, stop_cmd(), qi::space_type > start;
	};

}


#endif


