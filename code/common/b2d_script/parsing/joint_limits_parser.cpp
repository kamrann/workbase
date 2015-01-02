// joint_limits_parser.cpp

#include "joint_limits_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::joint_limits,
	(b2dc::rotation_joint::value_t, lower)
	(b2dc::rotation_joint::value_t, upper)
	);

namespace b2ds {

	template < typename Iterator >
	joint_limits_parser< Iterator >::joint_limits_parser():
		base_type(start_)
	{
		using qi::lit;
		using qi::double_;

		start_ =
			lit("limits") >>
			lit(ArgListOpen) >>
			double_ >>
			lit(',') >>
			double_ >>
			lit(ArgListClose)
			;
	}

	template struct joint_limits_parser < std::string::const_iterator > ;

}




