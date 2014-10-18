// get_cmd_parser.cpp

#include "get_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	sys_control::get_cmd,
	(prm::enum_param_val, values)
	);

namespace sys_control {

	template < typename Iterator >
	get_cmd_parser< Iterator >::get_cmd_parser():
		get_cmd_parser::base_type(start)
	{
		using qi::lit;

		start =
			lit("get") >>
			enum_vals
			;
	}

	template struct get_cmd_parser < std::string::const_iterator > ;

}




