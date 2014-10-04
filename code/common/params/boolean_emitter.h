// boolean_emitter.h

#ifndef __WB_PARAMS_BOOLEAN_EMITTER_H
#define __WB_PARAMS_BOOLEAN_EMITTER_H

#include <boost/spirit/include/karma.hpp>


namespace karma = boost::spirit::karma;

namespace prm {

	template < typename OutputIterator >
	struct boolean_emitter: karma::grammar< OutputIterator, bool() >
	{
		boolean_emitter(): boolean_emitter::base_type(start)
		{
			using karma::bool_;

			start %= bool_;
		}

		karma::rule< OutputIterator, bool() > start;
	};

}


#endif


