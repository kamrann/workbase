// random_emitter.h

#ifndef __WB_DDL_RANDOM_EMITTER_H
#define __WB_DDL_RANDOM_EMITTER_H

#include "param.h"
#include "value_parsing_ast.h"

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/adt.hpp>


namespace karma = boost::spirit::karma;
namespace phx = boost::phoenix;

namespace ddl {

	template < typename OutputIterator >
	struct random_emitter: karma::grammar< OutputIterator, random() >
	{
		random_emitter(): random_emitter::base_type(start, "random")
		{
			using karma::lit;
			using karma::double_;
			using karma::_1;
			using karma::_val;

			random_range %= double_ << ", " << double_;
			random_value %= double_;

			start =
				lit("[") <<
				(random_range | random_value)
				[
					_1 = phx::bind(&random::range, _val)
				] <<
				lit("]");

			start.name("random start");
			random_range.name("random range");
			random_value.name("random value");

			//karma::debug(start);
			//karma::debug(random_range);
			//karma::debug(random_value);
		}

		karma::rule< OutputIterator, double() > random_value;
		karma::rule< OutputIterator, std::pair< double, double >() > random_range;
		karma::rule< OutputIterator, random() > start;
	};

}


#endif


