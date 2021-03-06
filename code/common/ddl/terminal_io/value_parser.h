// value_parser.h

#ifndef __WB_DDL_VALUE_PARSER_H
#define __WB_DDL_VALUE_PARSER_H

#include "boolean_parser.h"
#include "integer_parser.h"
#include "realnum_parser.h"
#include "string_parser.h"
#include "enum_parser.h"
//#include "vector_parser.h"
//#include "random_parser.h"

#include "../values/value.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace ddl {

	template < typename Iterator >
	struct value_parser: qi::grammar< Iterator, value(), qi::space_type >
	{
		value_parser(): value_parser::base_type(start)
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

		typename boolean_parser< Iterator >::strict boolean;
		typename integer_parser< Iterator >::strict integer;
		typename realnum_parser< Iterator >::strict realnum;
		typename enum_parser< Iterator >::strict enumeration;
		typename string_parser< Iterator >::strict string_;
//		typename vec2_parser< Iterator >::strict vec2_;
//		typename random_parser< Iterator >::strict random_;

		qi::rule< Iterator, value(), qi::space_type > start;
	};

}


#endif


