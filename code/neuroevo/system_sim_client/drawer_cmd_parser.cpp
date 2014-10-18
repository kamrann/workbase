// drawer_cmd_parser.cpp

#include "drawer_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


namespace phx = boost::phoenix;

namespace sys_control {

	template < typename Iterator >
	drawer_cmd_parser< Iterator >::drawer_cmd_parser():
		drawer_cmd_parser::base_type(start)
	{
		using qi::lit;

		start =
			lit("draw")[phx::nothing]
			;
	}

	template struct drawer_cmd_parser < std::string::const_iterator > ;

}




