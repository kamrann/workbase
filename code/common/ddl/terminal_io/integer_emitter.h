// integer_emitter.h

#ifndef __WB_DDL_INTEGER_EMITTER_H
#define __WB_DDL_INTEGER_EMITTER_H

#include <boost/spirit/include/karma.hpp>

#include <cstdint>


namespace karma = boost::spirit::karma;

namespace ddl {

	template < typename OutputIterator >
	struct integer_emitter: karma::grammar< OutputIterator, intmax_t() >
	{
		integer_emitter(): integer_emitter::base_type(start)
		{
			using karma::int_;
						
			start %= int_;
		}

		karma::rule< OutputIterator, intmax_t() > start;
	};

}


#endif


