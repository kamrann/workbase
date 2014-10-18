// stop_cmd_parser.cpp

#include "stop_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	sys_control::stop_cmd,
	);

namespace phx = boost::phoenix;

namespace sys_control {

	template < typename Iterator >
	stop_cmd_parser< Iterator >::stop_cmd_parser():
		stop_cmd_parser::base_type(start)
	{
		using qi::lit;

		start =
			lit("stop")[ phx::nothing ]
			;
	}

	template struct stop_cmd_parser < std::string::const_iterator > ;

}




