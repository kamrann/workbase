// reset_cmd_parser.cpp

#include "reset_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	sys_control::reset_cmd::seed_data,
	(sys_control::reset_cmd::seed_data::SeedMethod, method)
	(boost::optional< unsigned int >, value)
	);

BOOST_FUSION_ADAPT_STRUCT(
	sys_control::reset_cmd,
	(sys_control::reset_cmd::seed_data, seed)
	);

namespace phx = boost::phoenix;

namespace sys_control {

	template < typename Iterator >
	reset_cmd_parser< Iterator >::reset_cmd_parser():
		reset_cmd_parser::base_type(start)
	{
		using qi::lit;
		using qi::uint_;
		using qi::eps;
		using qi::_val;
		using qi::_1;

		seed =
			(lit("seed") >>
			(
			uint_[_val = phx::construct< reset_cmd::seed_data >(_1)]
			| lit("same")[_val = reset_cmd::seed_data{ reset_cmd::seed_data::same_tag{} }]
			| lit("rand")[_val = reset_cmd::seed_data{ reset_cmd::seed_data::random_tag{} }]
			))
			| eps[phx::nothing]	// default constructed seed_data
			;

		start =
			lit("reset") >>
			seed
			;
	}

	template struct reset_cmd_parser < std::string::const_iterator > ;

}




