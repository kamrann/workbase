// boolean_parser.h

#ifndef __WB_PARAMS_BOOLEAN_PARSER_H
#define __WB_PARAMS_BOOLEAN_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	template < typename Iterator >
	struct boolean_parser: qi::grammar< Iterator, bool(), qi::space_type >
	{
		boolean_parser(): boolean_parser::base_type(start)
		{
			using qi::bool_;

			// TODO: expand to allow t/f or 1/0?
			start %= bool_;
		}

		qi::rule< Iterator, bool(), qi::space_type > start;
	};

}


#endif


