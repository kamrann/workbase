// integer_emitter.h

#ifndef __WB_PARAMS_INTEGER_EMITTER_H
#define __WB_PARAMS_INTEGER_EMITTER_H

#include <boost/spirit/include/karma.hpp>


namespace karma = boost::spirit::karma;

namespace prm {

	template < typename OutputIterator >
	struct integer_emitter: karma::grammar< OutputIterator, int() >
	{
		integer_emitter(): integer_emitter::base_type(start)
		{
			using karma::int_;
						
			start %= int_;
		}

		karma::rule< OutputIterator, int() > start;
	};

}


#endif


