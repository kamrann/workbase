// realnum_emitter.h

#ifndef __WB_DDL_REALNUM_EMITTER_H
#define __WB_DDL_REALNUM_EMITTER_H

#include <boost/spirit/include/karma.hpp>


namespace karma = boost::spirit::karma;

namespace ddl {

	template < typename OutputIterator >
	struct realnum_emitter: karma::grammar< OutputIterator, double() >
	{
		realnum_emitter(): realnum_emitter::base_type(start)
		{
			using karma::double_;
						
			start %= double_;
		}

		karma::rule< OutputIterator, double() > start;
	};

}


#endif


