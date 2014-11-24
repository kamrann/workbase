// chart_cmd_parser.cpp

#include "chart_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ga_control::chart_cmd
//	, (prm::enum_param_val, values)
	);

namespace phx = boost::phoenix;

namespace ga_control {

	template < typename Iterator >
	chart_cmd_parser< Iterator >::chart_cmd_parser():
		chart_cmd_parser::base_type(start)
	{
		using qi::lit;

		start =
			lit("chart")
			[phx::nothing]
//			>> enum_vals
			;
	}

	template struct chart_cmd_parser < std::string::const_iterator > ;

}




