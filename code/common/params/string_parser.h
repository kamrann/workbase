// string_parser.h

#ifndef __WB_PARAMS_STRING_PARSER_H
#define __WB_PARAMS_STRING_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	template < typename Iterator >
	struct string_parser: qi::grammar< Iterator, std::string(), qi::space_type >
	{
		string_parser(): string_parser::base_type(start)
		{
			using qi::lit;
			using qi::char_;
			using qi::lexeme;

			// TODO: also single quote
			start %= lit('\"') >> lexeme[*(char_ - lit('\"'))] >> lit('\"');
		}

		qi::rule< Iterator, std::string(), qi::space_type > start;
	};

}


#endif


