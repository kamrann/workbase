// repeat_add_cmd_parser.cpp

#include "repeat_add_cmd_parser.h"

#include <boost/spirit/include/phoenix.hpp>


namespace phx = boost::phoenix;

namespace ddl {

	template < typename Iterator >
	repeat_add_cmd_parser< Iterator >::repeat_add_cmd_parser():
		repeat_add_cmd_parser::base_type(start)
	{
		using qi::lit;
		using qi::_val;

		start =
			lit("add")
			[
				_val = phx::val(repeat_add_cmd{})
			]
		;
	}

	template struct repeat_add_cmd_parser < std::string::const_iterator > ;

}



