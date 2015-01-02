// name_parser.h

#ifndef __WB_B2D_SCRIPT_NAME_PARSER_H
#define __WB_B2D_SCRIPT_NAME_PARSER_H

#include <boost/spirit/include/qi.hpp>

#include <string>


namespace qi = boost::spirit::qi;

namespace b2ds {

	template < typename Iterator >
	struct name_parser: qi::grammar< Iterator, std::string(), qi::space_type >
	{
		name_parser();

		qi::rule< Iterator, std::string(), qi::space_type > start_;
	};

}


#endif


