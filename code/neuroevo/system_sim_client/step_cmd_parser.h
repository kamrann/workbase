// step_cmd_parser.h

#ifndef __WB_SYSSIM_CLIENT_STEP_CMD_PARSER_H
#define __WB_SYSSIM_CLIENT_STEP_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace sys_control {

	struct step_cmd
	{
		boost::optional< unsigned int > frames;
	};

	template < typename Iterator >
	struct step_cmd_parser: qi::grammar< Iterator, step_cmd(), qi::space_type >
	{
		step_cmd_parser();

		qi::rule< Iterator, step_cmd(), qi::space_type > start;
	};

}


#endif


