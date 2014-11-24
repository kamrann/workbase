// store_cmd_parser.cpp

#include "store_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ga_control::store_cmd,
	(size_t, index)
	(boost::optional< std::string >, name)
	);

namespace phx = boost::phoenix;

namespace ga_control {

	template < typename Iterator >
	store_cmd_parser< Iterator >::store_cmd_parser():
		store_cmd_parser::base_type(start)
	{
		using qi::lit;
		using qi::uint_;
		using qi::char_;
		using qi::alnum;

		name_ =
			+(qi::alnum | char_('_') | char_('-'))	// TODO:
			;

		start =
			lit("store")
			>> uint_
			>> -(lit("as") >> name_)
			;
	}

	template struct store_cmd_parser < std::string::const_iterator > ;

}




