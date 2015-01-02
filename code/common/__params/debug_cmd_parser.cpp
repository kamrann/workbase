// debug_cmd_parser.cpp

#include "debug_cmd_parser.h"

#include <boost/spirit/include/phoenix.hpp>


namespace phx = boost::phoenix;

namespace prm {

	template < typename Iterator >
	debug_cmd_parser< Iterator >::debug_cmd_parser():
		debug_cmd_parser::base_type(start)
	{
		using qi::lit;
		using qi::_val;

		start =
			lit("dbg")
			[
				_val = phx::val(debug_cmd{})
			]
		;
	}

	template struct debug_cmd_parser < std::string::const_iterator > ;

}




