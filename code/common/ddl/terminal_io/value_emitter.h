// value_emitter.h

#ifndef __WB_DDL_VALUE_EMITTER_H
#define __WB_DDL_VALUE_EMITTER_H

#include "boolean_emitter.h"
#include "integer_emitter.h"
#include "realnum_emitter.h"
#include "string_emitter.h"
#include "enum_emitter.h"
//#include "vector_emitter.h"
//#include "random_emitter.h"

#include "../values/value.h"

#include <boost/spirit/include/karma.hpp>


namespace karma = boost::spirit::karma;

namespace ddl {

	template < typename OutputIterator >
	struct value_emitter: karma::grammar< OutputIterator, value() >
	{
		value_emitter(): value_emitter::base_type(start)
		{
			start %=
				boolean
				| realnum
				| integer
//				| vec2_
//				| random_
				| enumeration
				| string_
				;
		}

		boolean_emitter< OutputIterator > boolean;
		integer_emitter< OutputIterator > integer;
		realnum_emitter< OutputIterator > realnum;
		enum_emitter< OutputIterator > enumeration;
		string_emitter< OutputIterator > string_;
//		vec2_emitter< OutputIterator > vec2_;
//		random_emitter< OutputIterator > random_;

		karma::rule< OutputIterator, value() > start;
	};

}


#endif


