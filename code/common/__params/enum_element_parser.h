// enum_element_parser.h

#ifndef __WB_PARAMS_ENUM_ELEMENT_PARSER_H
#define __WB_PARAMS_ENUM_ELEMENT_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	template < typename Iterator >
	struct enum_element_parser: qi::grammar< Iterator, std::string(), qi::space_type >
	{
		enum_element_parser(): enum_element_parser::base_type(start)
		{
			using qi::char_;
			using qi::lexeme;

			start = lexeme[ +(char_ - char_(" ,{}")) ];
		}

		qi::rule< Iterator, std::string(), qi::space_type > start;
	};

}


#endif


