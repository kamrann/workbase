// save_cmd_parser.cpp

#include "save_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ddl::save_cmd,
	(boost::optional< ddl::nav_component >, nav)
	(std::string, filename)
	);

namespace ddl {

	template < typename Iterator >
	save_cmd_parser< Iterator >::save_cmd_parser(bool enable_indexed):
		save_cmd_parser::base_type(start),
		unrestricted_nav{ false, enable_indexed }
	{
		using qi::lit;
		using qi::char_;
		using qi::alpha;
		using qi::lexeme;
		using qi::hold;

		// TODO: quick hack
		prefix =
			alpha >>
			char_(':')
			;

		root =
			char_('/')
			;

		path_element =
			+(char_ - char_(R"(\/?<>|:*")"))
			;

		filename =
			lexeme
			[
				// todo: requiring quotes to make sure the filename is not swallowed by the optional nav
				// probably another way around this is quoting is annoying
				lit('\"') >>
				-hold[ prefix ] >>
				-root >>
				-(path_element >> *(char_('/') >> path_element)) >>
				lit('\"')
			];
		//

		start =
			lit("save") >>
			-unrestricted_nav >>
			filename
			;
	}

	template struct save_cmd_parser < std::string::const_iterator > ;

}




