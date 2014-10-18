// cmd_parser.h

#ifndef __WB_SYSSIM_CLIENT_CMD_PARSER_H
#define __WB_SYSSIM_CLIENT_CMD_PARSER_H

#include "reset_cmd_parser.h"
#include "step_cmd_parser.h"
#include "run_cmd_parser.h"
#include "stop_cmd_parser.h"
#include "get_cmd_parser.h"
#include "chart_cmd_parser.h"
#include "drawer_cmd_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>


namespace qi = boost::spirit::qi;

namespace sys_control {

	typedef boost::variant <
		reset_cmd
		, step_cmd
		, run_cmd
		, stop_cmd
		, get_cmd
		, chart_cmd
		, drawer_cmd
	> command;

	template < typename Iterator >
	struct cmd_parser: qi::grammar< Iterator, command(), qi::space_type >
	{
		cmd_parser();

		qi::rule< Iterator, command(), qi::space_type > start;

		reset_cmd_parser< Iterator > reset;
		step_cmd_parser< Iterator > step;
		run_cmd_parser< Iterator > run;
		stop_cmd_parser< Iterator > stop;
		get_cmd_parser< Iterator > get;
		chart_cmd_parser< Iterator > chart;
		drawer_cmd_parser< Iterator > drawer;
	};

}


#endif


