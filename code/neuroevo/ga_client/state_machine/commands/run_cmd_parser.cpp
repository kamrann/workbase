// run_cmd_parser.cpp

#include "run_cmd_parser.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ga_control::run_cmd,
	(boost::optional< double >, frame_rate)
	);

namespace phx = boost::phoenix;

namespace ga_control {

	template < typename Iterator >
	run_cmd_parser< Iterator >::run_cmd_parser():
		run_cmd_parser::base_type(start)
	{
		using qi::lit;
		using qi::double_;
		using qi::_val;

		realtime =
			(lit("realtime") | lit("rt"))
			[
				_val = 0.0
			];

		start =
			lit("run") >>
			-(realtime | double_)
			;
	}

	template struct run_cmd_parser < std::string::const_iterator > ;

}




