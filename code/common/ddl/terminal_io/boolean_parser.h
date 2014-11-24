// boolean_parser.h

#ifndef __WB_PARAMS_BOOLEAN_PARSER_H
#define __WB_PARAMS_BOOLEAN_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	template < typename Iterator >
	struct boolean_parser
	{
		struct core: qi::grammar < Iterator, bool(), qi::space_type >
		{
			core(): core::base_type(start)
			{
				using qi::bool_;

				// TODO: expand to allow t/f or 1/0?
				start %= bool_;
			}

			qi::rule< Iterator, bool(), qi::space_type > start;
		};

		typedef core strict, lax;
	};

}


#endif


