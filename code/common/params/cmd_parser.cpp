// cmd_parser.cpp

#include "cmd_parser.h"


namespace prm {

	template < typename Iterator >
	cmd_parser< Iterator >::cmd_parser(bool is_terminal):
		cmd_parser::base_type(start),
		nav{ !is_terminal },
		list{ !is_terminal },
		setvalue{ !is_terminal, !is_terminal },
		save{ !is_terminal },
		load{ !is_terminal }
	{
		using qi::eoi;

		start =
			(
			nav
			| list
			| setvalue
			| repeat_add
			| repeat_rm
			| save
			| load
			| dbg
			)
			>>
			eoi
			;
	}

	template struct cmd_parser < std::string::const_iterator > ;

}



