// enum_remove_cmd_parser.cpp

#include "enum_remove_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	prm::enum_remove_cmd,
	(std::vector< std::string >, elements)
	);

namespace prm {

	template < typename Iterator >
	enum_remove_cmd_parser< Iterator >::enum_remove_cmd_parser():
		enum_remove_cmd_parser::base_type(start)
	{
		using qi::lit;

		start =
			(lit("remove") | lit("rm")) >>
			+element
			;
	}

	template struct enum_remove_cmd_parser < std::string::const_iterator > ;

}



