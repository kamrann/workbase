// pop_cmd_parser.cpp

#include "pop_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ga_control::pop_cmd,
	);

namespace phx = boost::phoenix;

namespace ga_control {

	template < typename Iterator >
	pop_cmd_parser< Iterator >::pop_cmd_parser():
		pop_cmd_parser::base_type(start)
	{
		using qi::lit;

		start =
			lit("pop")[phx::nothing]
			;
	}

	template struct pop_cmd_parser < std::string::const_iterator > ;

}




