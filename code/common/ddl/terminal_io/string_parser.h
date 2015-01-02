// string_parser.h

#ifndef __WB_DDL_STRING_PARSER_H
#define __WB_DDL_STRING_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	template < typename Iterator >
	struct string_parser
	{
		static const char quote_ch = '\"';

		struct strict: qi::grammar < Iterator, std::string(), qi::space_type >
		{
			strict(): strict::base_type(start)
			{
				using qi::lit;
				using qi::char_;
				using qi::lexeme;

				start =
					lexeme
					[
						lit(quote_ch) >>
						*(char_ - lit(quote_ch)) >>
						lit(quote_ch)
					];
			}

			qi::rule< Iterator, std::string(), qi::space_type > start;
		};

		struct lax: qi::grammar < Iterator, std::string(), qi::space_type >
		{
			lax(): lax::base_type(start)
			{
				using qi::string_;

				start =
					str
					| string_;
			}

			qi::rule< Iterator, std::string(), qi::space_type > start;

			strict str;
		};
	};

}


#endif


