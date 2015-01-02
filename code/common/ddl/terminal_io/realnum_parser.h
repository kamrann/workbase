// realnum_parser.h

#ifndef __WB_DDL_REALNUM_PARSER_H
#define __WB_DDL_REALNUM_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>


namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

namespace ddl {

	namespace detail {

		inline double convert_double(std::string str)
		{
			return std::stod(str);
		}

	}

	template < typename Iterator >
	struct realnum_parser
	{
		template < typename T >
		struct strict_real_policies: qi::real_policies< T >
		{
			static bool const expect_dot = true;
		};

		struct strict: qi::grammar < Iterator, double(), qi::space_type >
		{
			strict(): strict::base_type(start)
			{
				start =
					real
					;
			}

			qi::real_parser< double, strict_real_policies< double > > real;
			qi::rule< Iterator, double(), qi::space_type > start;
		};

		struct lax: qi::grammar < Iterator, double(), qi::space_type >
		{
			lax(): lax::base_type(start)
			{
				using qi::double_;

				start =
					double_
					;
			}

			qi::rule< Iterator, double(), qi::space_type > start;
		};
	};

}

/*
using qi::lexeme;
using qi::matches;
using qi::digit;
using qi::string;
using qi::char_;
using qi::_val;
using qi::_1;

// For strict parsing, as used when parsing a value when the type isn't known in advance, need to
// differentiate real number from integer unambiguously. Therefore, require decimal point.
minus =
string("-")
;

before =
string("0")
| char_("1-9") >> *digit
;

dp =
string(".")
;

after =
*digit
;

value_as_string =
-minus >> -before >> dp >> after
;

start = value_as_string
[
_val = phx::bind(&detail::convert_double, _1)
];

qi::rule< Iterator, std::string() > minus;
qi::rule< Iterator, std::string() > before;
qi::rule< Iterator, std::string() > dp;
qi::rule< Iterator, std::string() > after;
qi::rule< Iterator, std::string() > value_as_string;

*/



#endif


