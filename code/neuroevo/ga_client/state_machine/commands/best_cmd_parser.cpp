// best_cmd_parser.cpp

#include "best_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ga_control::best_cmd,
	);

namespace phx = boost::phoenix;

namespace ga_control {

	template < typename Iterator >
	best_cmd_parser< Iterator >::best_cmd_parser():
		best_cmd_parser::base_type(start)
	{
		using qi::lit;

		start =
			lit("best")[phx::nothing]
			;
	}

	template struct best_cmd_parser < std::string::const_iterator > ;

}




