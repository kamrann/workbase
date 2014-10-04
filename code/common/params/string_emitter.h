// string_emitter.h

#ifndef __WB_PARAMS_STRING_EMITTER_H
#define __WB_PARAMS_STRING_EMITTER_H

#include <boost/spirit/include/karma.hpp>


namespace karma = boost::spirit::karma;

namespace prm {

	template < typename OutputIterator >
	struct string_emitter: karma::grammar< OutputIterator, std::string() >
	{
		string_emitter(): string_emitter::base_type(start)
		{
			using karma::lit;
			using karma::string;

			// TODO: also single quote
			start %= lit('\"') << string << lit('\"');
		}

		karma::rule< OutputIterator, std::string() > start;
	};

}


#endif


