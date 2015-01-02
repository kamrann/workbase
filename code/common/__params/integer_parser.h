// integer_parser.h

#ifndef __WB_PARAMS_INTEGER_PARSER_H
#define __WB_PARAMS_INTEGER_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	template < typename Iterator >
	struct integer_parser
	{
		struct core: qi::grammar < Iterator, int(), qi::space_type >
		{
			core(): core::base_type(start)
			{
				using qi::int_;

				start %= int_;
			}

			qi::rule< Iterator, int(), qi::space_type > start;
		};

		typedef core strict, lax;
	};

}


#endif


