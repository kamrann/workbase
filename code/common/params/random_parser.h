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
	struct random_parser: qi::grammar< Iterator, random(), qi::space_type >
	{
		random_parser(): random_parser::base_type(start)
		{
			using qi::lit;
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
				lit("[") >>
				(random_range | random_value) >>
				lit("]");
		}

		qi::rule< Iterator, random(), qi::space_type > random_value;
		qi::rule< Iterator, random(), qi::space_type > random_range;
		qi::rule< Iterator, random(), qi::space_type > start;
	};

}


#endif


