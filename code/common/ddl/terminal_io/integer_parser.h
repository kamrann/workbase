// integer_parser.h

#ifndef __WB_DDL_INTEGER_PARSER_H
#define __WB_DDL_INTEGER_PARSER_H

#include <boost/spirit/include/qi.hpp>

#include <cstdint>


namespace qi = boost::spirit::qi;

namespace ddl {

	template < typename Iterator >
	struct integer_parser
	{
		struct core: qi::grammar < Iterator, intmax_t(), qi::space_type >
		{
			core(): core::base_type(start)
			{
				using qi::int_;

				start %= int_;
			}

			qi::rule< Iterator, intmax_t(), qi::space_type > start;
		};

		typedef core strict, lax;
	};

}


#endif


