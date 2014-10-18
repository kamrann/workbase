// setvalue_cmd_parser.cpp

#include "setvalue_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	prm::setvalue_cmd,
	(boost::optional< prm::nav_component >, nav)
	(prm::param, val)
	);

namespace prm {

	template < typename Iterator >
	setvalue_cmd_parser< Iterator >::setvalue_cmd_parser(bool enable_indexed, bool require_nav):
		setvalue_cmd_parser::base_type(start),
		unrestricted_nav{ false, enable_indexed }
	{
		using qi::lit;

		if(require_nav)
		{
			nav = unrestricted_nav;
		}
		else
		{
			nav = -unrestricted_nav;
		}

		start =
			lit("set") >>
			nav >>
			param_value_parser
			;
	}

	template struct setvalue_cmd_parser < std::string::const_iterator > ;

}




