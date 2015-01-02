// sucker_parser.cpp

#include "sucker_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::sucker,
	(boost::optional< std::string >, name)
	(b2ds::ast::rel_coord2d, location)
	);


namespace b2ds {

	template < typename Iterator >
	sucker_parser< Iterator >::sucker_parser():
		base_type(start_)
	{
		using qi::lit;

		start_ =
			lit("sucker") >>
			-(lit(':') >> name_parser_) >>
			lit(ArgListOpen) >>

			rel_coord_parser_ >>

			lit(ArgListClose)
			;
	}

	template struct sucker_parser < std::string::const_iterator > ;

}




