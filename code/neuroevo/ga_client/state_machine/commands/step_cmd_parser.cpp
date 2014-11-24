// step_cmd_parser.cpp

#include "step_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ga_control::step_cmd,
	(boost::optional< unsigned int >, generations)
	);

namespace ga_control {

	template < typename Iterator >
	step_cmd_parser< Iterator >::step_cmd_parser():
		step_cmd_parser::base_type(start)
	{
		using qi::lit;
		using qi::uint_;

		start =
			lit("step") >>
			-uint_
			;
	}

	template struct step_cmd_parser < std::string::const_iterator > ;

}




