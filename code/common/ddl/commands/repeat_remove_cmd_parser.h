// repeat_remove_cmd_parser.h

#ifndef __WB_PARAMS_REPEAT_REMOVE_CMD_PARSER_H
#define __WB_PARAMS_REPEAT_REMOVE_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	struct repeat_remove_cmd
	{
		unsigned int index;

		repeat_remove_cmd() {}
		repeat_remove_cmd(unsigned int _idx):
			index{ _idx }
		{}
	};

	template < typename Iterator >
	struct repeat_remove_cmd_parser: qi::grammar< Iterator, repeat_remove_cmd(), qi::space_type >
	{
		repeat_remove_cmd_parser();

		qi::rule< Iterator, repeat_remove_cmd(), qi::space_type > start;
	};

}


#endif


