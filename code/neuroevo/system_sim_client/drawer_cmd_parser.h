// drawer_cmd_parser.h

#ifndef __WB_SYSSIM_CLIENT_DRAWER_CMD_PARSER_H
#define __WB_SYSSIM_CLIENT_DRAWER_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace sys_control {

	struct drawer_cmd
	{
		boost::optional< double > zoom;
	};

	template < typename Iterator >
	struct drawer_cmd_parser: qi::grammar< Iterator, drawer_cmd(), qi::space_type >
	{
		drawer_cmd_parser();

		qi::rule< Iterator, drawer_cmd(), qi::space_type > start;
	};

}


#endif


