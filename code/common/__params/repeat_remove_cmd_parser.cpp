// repeat_remove_cmd_parser.cpp

#include "repeat_remove_cmd_parser.h"

#include <boost/spirit/include/phoenix.hpp>


namespace phx = boost::phoenix;

namespace prm {

	template < typename Iterator >
	repeat_remove_cmd_parser< Iterator >::repeat_remove_cmd_parser():
		repeat_remove_cmd_parser::base_type(start)
	{
		using qi::lit;
		using qi::uint_;
		using qi::_val;
		using qi::_1;

		start =
			(lit("remove") | lit("rm") | lit("r")) >>
			uint_
			[
				_val = phx::construct< repeat_remove_cmd >(_1 - phx::val(1u))
			]
		;
	}

	template struct repeat_remove_cmd_parser < std::string::const_iterator > ;

}



