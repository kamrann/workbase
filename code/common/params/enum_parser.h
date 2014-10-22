// enum_parser.h

#ifndef __WB_PARAMS_ENUM_PARSER_H
#define __WB_PARAMS_ENUM_PARSER_H

#include "enum_element_parser.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	template < typename Iterator >
	struct enum_parser
	{
		static const char open_ch = '{';
		static const char close_ch = '}';

		struct core: qi::grammar < Iterator, enum_param_val(), qi::space_type >
		{
			core(): core::base_type(start)
			{
				start = (element % ',');
			}

			qi::rule< Iterator, enum_param_val(), qi::space_type > start;

			enum_element_parser< Iterator > element;
		};

		struct strict: qi::grammar < Iterator, enum_param_val(), qi::space_type >
		{
			strict(): strict::base_type(start)
			{
				using qi::lit;

				start =
					lit(open_ch) >> -cr >> lit(close_ch);
			}

			qi::rule< Iterator, enum_param_val(), qi::space_type > start;

			core cr;
		};

		struct lax: qi::grammar < Iterator, enum_param_val(), qi::space_type >
		{
			lax(): lax::base_type(start)
			{
				start =
					str
					| cr
					;
			}

			qi::rule< Iterator, enum_param_val(), qi::space_type > start;

			strict str;
			core cr;
		};
	};

}


#endif


