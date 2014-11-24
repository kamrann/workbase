// run_cmd_parser.h

#ifndef __WB_GA_CLIENT_RUN_CMD_PARSER_H
#define __WB_GA_CLIENT_RUN_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace ga_control {

	struct run_cmd
	{
		boost::optional< double > frame_rate;
	};

	template < typename Iterator >
	struct run_cmd_parser: qi::grammar< Iterator, run_cmd(), qi::space_type >
	{
		run_cmd_parser();

		qi::rule< Iterator, double(), qi::space_type > realtime;
		qi::rule< Iterator, run_cmd(), qi::space_type > start;
	};

}


#endif


