// enum_emitter.h

#ifndef __WB_PARAMS_ENUM_EMITTER_H
#define __WB_PARAMS_ENUM_EMITTER_H

#include "../values/value.h"

#include <boost/spirit/include/karma.hpp>


namespace karma = boost::spirit::karma;

namespace ddl {

	template < typename OutputIterator >
	struct enum_emitter: karma::grammar< OutputIterator, enum_param_val() >
	{
		enum_emitter(): enum_emitter::base_type(start)
		{
			using karma::lit;
			using karma::string;

			enum_value %= string;
			
			start %= lit("{") << -(" " << (enum_value % ", ") << " ") << lit("}");
		}

		karma::rule< OutputIterator, std::string() > enum_value;
		karma::rule< OutputIterator, enum_param_val() > start;
	};

}


#endif


