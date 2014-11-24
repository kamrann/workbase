// list_cmd_parser.h

#ifndef __WB_PARAMS_LIST_CMD_PARSER_H
#define __WB_PARAMS_LIST_CMD_PARSER_H

#include "nav_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	struct depth_type
	{
		depth_type():
			value(1)
		{}
		depth_type(boost::optional< unsigned int > opt):
			value(opt ? *opt : std::numeric_limits< unsigned int >::max())
		{}

		unsigned int value;
	};

	struct list_cmd
	{
		bool required_only;
		bool type_info;
		bool expand_imports;
		boost::optional< nav_component > nav;
		boost::optional< depth_type > depth;
	};

	template < typename Iterator >
	struct list_cmd_parser: qi::grammar< Iterator, list_cmd(), qi::space_type >
	{
		struct list_symbols: public qi::symbols < char, bool >
		{
			list_symbols();
		};

		list_cmd_parser(bool enable_indexed);

		qi::rule< Iterator, depth_type(), qi::space_type > depth;
		qi::rule< Iterator, list_cmd(), qi::space_type > start;

		nav_parser< Iterator > unrestricted_nav;

		list_symbols list_sym;
	};

}


#endif


