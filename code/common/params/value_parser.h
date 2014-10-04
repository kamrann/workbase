// value_parser.h

#ifndef __WB_PARAMS_VALUE_PARSER_H
#define __WB_PARAMS_VALUE_PARSER_H

#include "param.h"
//#include "value_parsing_ast.h"
#include "boolean_parser.h"
#include "integer_parser.h"
#include "realnum_parser.h"
#include "enum_parser.h"
#include "string_parser.h"
#include "vector_parser.h"
#include "random_parser.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	template < typename Iterator >
	struct value_parser: qi::grammar< Iterator, param(), qi::space_type >
	{
		value_parser(): value_parser::base_type(start)
		{
			start %=
				boolean
				| realnum
				| integer
				| vec2_
				| random_
				| enumeration
				| string_
				;
		}

		boolean_parser< Iterator > boolean;
		integer_parser< Iterator > integer;
		realnum_parser< Iterator > realnum;
		enum_parser< Iterator > enumeration;
		string_parser< Iterator > string_;
		vec2_parser< Iterator > vec2_;
		random_parser< Iterator > random_;

		qi::rule< Iterator, param(), qi::space_type > start;
	};

}


#endif


