// chart_cmd_parser.h

#ifndef __WB_GA_CLIENT_CHART_CMD_PARSER_H
#define __WB_GA_CLIENT_CHART_CMD_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace ga_control {

	struct chart_cmd
	{
		//prm::enum_param_val values;
	};

	template < typename Iterator >
	struct chart_cmd_parser: qi::grammar< Iterator, chart_cmd(), qi::space_type >
	{
		chart_cmd_parser();

		qi::rule< Iterator, chart_cmd(), qi::space_type > start;

		//typename prm::enum_parser< Iterator >::lax enum_vals;
	};

}


#endif


