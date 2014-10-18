// nav_cmd_parser.cpp

#include "nav_cmd_parser.h"


namespace prm {

	template < typename Iterator >
	nav_cmd_parser< Iterator >::nav_cmd_parser(bool enable_indexed):
		nav_cmd_parser::base_type(start),
		restricted_nav{ true, enable_indexed },
		unrestricted_nav{ false, enable_indexed }
	{
		using qi::lit;

		start =
			(
			(lit("move") | lit("mv") | lit("m"))
			>>
			unrestricted_nav
			)
			|
			restricted_nav
			;
	}

	template struct nav_cmd_parser < std::string::const_iterator > ;

}




