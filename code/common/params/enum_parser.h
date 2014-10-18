// enum_parser.h

#ifndef __WB_PARAMS_ENUM_PARSER_H
#define __WB_PARAMS_ENUM_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	template < typename Iterator >
	struct enum_parser: qi::grammar< Iterator, enum_param_val(), qi::space_type >
	{
		enum_parser(): enum_parser::base_type(start)
		{
			using qi::lit;
			using qi::alpha;
			using qi::alnum;
			using qi::char_;
			using qi::lexeme;

			enum_value = +(char_ - char_(" ,{}"));
				//lexeme[alpha >> *(alnum | char_('_'))];
			
			start = lit("{") >> -(enum_value % ',') >> lit("}");
		}

		qi::rule< Iterator, std::string(), qi::space_type > enum_value;
		qi::rule< Iterator, enum_param_val(), qi::space_type > start;
	};

}


#endif


