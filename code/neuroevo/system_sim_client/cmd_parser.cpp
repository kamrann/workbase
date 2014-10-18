// cmd_parser.cpp

#include "cmd_parser.h"


namespace sys_control {

	template < typename Iterator >
	cmd_parser< Iterator >::cmd_parser():
		cmd_parser::base_type(start)
	{
		using qi::eoi;

		start =
			(
			reset
			| step
			| run
			| stop
			| get
			| chart
			| drawer
			)
			>>
			eoi
			;
	}

	template struct cmd_parser < std::string::const_iterator > ;

}



