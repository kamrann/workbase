// cmd_parser.h

#ifndef __WB_PARAMS_CMD_PARSER_H
#define __WB_PARAMS_CMD_PARSER_H

#include "nav_cmd_parser.h"
#include "list_cmd_parser.h"
#include "setvalue_cmd_parser.h"
#include "reset_cmd_parser.h"
#include "repeat_add_cmd_parser.h"
#include "repeat_remove_cmd_parser.h"
#include "save_cmd_parser.h"
#include "load_cmd_parser.h"
#include "debug_cmd_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	typedef boost::variant <
		nav_cmd
		, list_cmd
		, setvalue_cmd
		, reset_cmd
		, repeat_add_cmd
		, repeat_remove_cmd
		, save_cmd
		, load_cmd
		, debug_cmd
	> command;

	template < typename Iterator >
	struct cmd_parser: qi::grammar< Iterator, command(), qi::space_type >
	{
		cmd_parser(bool is_terminal);

		qi::rule< Iterator, command(), qi::space_type > start;

		nav_cmd_parser< Iterator > nav;
		list_cmd_parser< Iterator > list;
		setvalue_cmd_parser< Iterator > setvalue;
		reset_cmd_parser< Iterator > reset;
		repeat_add_cmd_parser< Iterator > repeat_add;
		repeat_remove_cmd_parser< Iterator > repeat_rm;
		save_cmd_parser< Iterator > save;
		load_cmd_parser< Iterator > load;
		debug_cmd_parser< Iterator > dbg;
	};

}


#endif


