// realnum_parser.h

#ifndef __WB_PARAMS_REALNUM_PARSER_H
#define __WB_PARAMS_REALNUM_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>


namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

namespace prm {

	namespace detail {

		inline double convert_double(std::string str)
		{
			return std::stod(str);
		}

	}

	template < typename Iterator >
	struct realnum_parser: qi::grammar< Iterator, double(), qi::space_type >
	{
		realnum_parser(): realnum_parser::base_type(start)
		{
			//using qi::double_;
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
		}

		qi::rule< Iterator, std::string() > minus;
		qi::rule< Iterator, std::string() > before;
		qi::rule< Iterator, std::string() > dp;
		qi::rule< Iterator, std::string() > after;
		qi::rule< Iterator, std::string() > value_as_string;
		qi::rule< Iterator, double(), qi::space_type > start;
	};

}


#endif


