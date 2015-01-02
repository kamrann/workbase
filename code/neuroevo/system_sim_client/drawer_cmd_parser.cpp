// drawer_cmd_parser.cpp

#include "drawer_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	sys_control::drawer_cmd,
	(boost::optional< double >, zoom)
	);

namespace phx = boost::phoenix;

namespace sys_control {

	template < typename Iterator >
	drawer_cmd_parser< Iterator >::drawer_cmd_parser():
		drawer_cmd_parser::base_type(start)
	{
		using qi::lit;
		using qi::double_;

		start =
			lit("draw")
			>> -(lit("x") >> double_)
			;
	}

	template struct drawer_cmd_parser < std::string::const_iterator > ;

}




