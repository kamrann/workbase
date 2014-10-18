// list_cmd_parser.cpp

#include "list_cmd_parser.h"

#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	prm::list_cmd,
	(bool, required_only)
	(bool, type_info)
	(bool, expand_imports)
	(boost::optional< prm::nav_component >, nav)
	(boost::optional< prm::depth_type >, depth)
	);

namespace phx = boost::phoenix;

namespace prm {

	template < typename Iterator >
	list_cmd_parser< Iterator >::list_symbols::list_symbols()
	{
		add
			("?", false)
			("!", true)
			;
	}

	template < typename Iterator >
	list_cmd_parser< Iterator >::list_cmd_parser(bool enable_indexed):
		list_cmd_parser::base_type(start),
		unrestricted_nav{ false, enable_indexed }
	{
		using qi::uint_;
		using qi::lit;
		using qi::matches;
		using qi::_val;
		using qi::_1;

		depth =
			lit(':') >>
			(uint_ | lit("all") | lit("a"))
			[
				_val = phx::construct< depth_type >(_1)
			];

		start =
			list_sym >>
			matches[lit('?')] >>
			matches[lit('+')] >>
			-unrestricted_nav >>
			-depth
			;
	}

	template struct list_cmd_parser < std::string::const_iterator > ;

}




