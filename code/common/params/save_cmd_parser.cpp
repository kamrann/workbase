// save_cmd_parser.cpp

#include "save_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	prm::save_cmd,
	(boost::optional< prm::nav_component >, nav)
	(std::string, filename)
	);

namespace prm {

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
				-hold[ prefix ] >>
				-root >>
				-(path_element >> *(char_('/') >> path_element))
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




