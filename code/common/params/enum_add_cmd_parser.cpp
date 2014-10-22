// enum_add_cmd_parser.cpp

#include "enum_add_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	prm::enum_add_cmd,
	(std::vector< std::string >, elements)
	);

namespace prm {

	template < typename Iterator >
	enum_add_cmd_parser< Iterator >::enum_add_cmd_parser():
		enum_add_cmd_parser::base_type(start)
	{
		using qi::lit;

		start =
			lit("add") >>
			+element
			;
	}

	template struct enum_add_cmd_parser < std::string::const_iterator > ;

}



