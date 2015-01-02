// random_parser.h

#ifndef __WB_PARAMS_RANDOM_PARSER_H
#define __WB_PARAMS_RANDOM_PARSER_H

#include "param.h"
#include "value_parsing_ast.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/std_pair.hpp>


namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

namespace prm {

	template < typename Iterator >
	struct random_parser
	{
		static const char open_ch = '[';
		static const char close_ch = ']';

		struct core: qi::grammar < Iterator, random(), qi::space_type >
		{
			core(): core::base_type(start)
			{
				using qi::double_;
				using qi::_1;
				using qi::_2;
				using qi::_val;

				random_value =
					double_
					;

				random_range =
					(double_ >> ',' >> double_)
					[
						_val = phx::construct< prm::random >(
						phx::construct< std::pair< double, double > >(
						_1, _2))
					];

				start =
					(random_range | random_value)
					;
			}

			qi::rule< Iterator, random(), qi::space_type > random_value;
			qi::rule< Iterator, random(), qi::space_type > random_range;
			qi::rule< Iterator, random(), qi::space_type > start;
		};

		struct strict: qi::grammar < Iterator, random(), qi::space_type >
		{
			strict(): strict::base_type(start)
			{
				using qi::lit;

				start =
					lit(open_ch) >>
					cr >>
					lit(close_ch);
			}

			qi::rule< Iterator, random(), qi::space_type > start;

			core cr;
		};

		struct lax: qi::grammar < Iterator, random(), qi::space_type >
		{
			lax(): lax::base_type(start)
			{
				start =
					str
					| cr;
			}

			qi::rule< Iterator, random(), qi::space_type > start;

			strict str;
			core cr;
		};
	};

}


#endif


