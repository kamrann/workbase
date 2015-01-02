// reset_cmd_parser.cpp

#include "reset_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ddl::reset_cmd,
	(boost::optional< ddl::nav_component >, nav)
	);

namespace ddl {

	template < typename Iterator >
	reset_cmd_parser< Iterator >::reset_cmd_parser(bool enable_indexed):
		reset_cmd_parser::base_type(start),
		unrestricted_nav{ false, enable_indexed }
	{
		using qi::lit;

		start =
			lit("reset") >>
			-unrestricted_nav
			;
	}

	template struct reset_cmd_parser < std::string::const_iterator > ;

}




