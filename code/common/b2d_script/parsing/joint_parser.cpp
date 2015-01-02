// joint_parser.cpp

#include "joint_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::joint_anchors,
	(b2ds::ast::rel_coord2d, b1)
	(b2ds::ast::rel_coord2d, b2)
	);

BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::speed_control,
	(boost::optional< double >, max_speed)
	);

BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::positional_control,
	(boost::optional< double >, gain)
	(boost::optional< double >, max_speed)
	);

BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::joint_props,
	(boost::optional< b2ds::ast::joint_anchors >, anchors)
	(boost::optional< b2ds::ast::joint_limits >, limits)
	(boost::optional< b2ds::ast::max_torque >, max_torque)
	(boost::optional< b2ds::ast::control >, control)
	);

BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::joint,
	(boost::optional< std::string >, name)
	(b2ds::ast::joint_props, props)
	);

namespace b2ds {

	namespace phx = boost::phoenix;

	template < typename Iterator >
	joint_parser< Iterator >::joint_parser():
		base_type(start_)
	{
		using qi::lit;
		using qi::double_;

		anchors_ =
			lit("bodies") >>
			lit(ArgListOpen) >>
			rel_coord_parser_ >>
			lit(',') >>
			rel_coord_parser_ >>
			lit(ArgListClose)
			;

		max_torque_ =
			lit("max_torque") >>
			lit(ArgListOpen) >>
			double_ >>
			lit(ArgListClose)
			;

		torque_control_ =
			(
			lit("torque") >>
			-(
			lit(ArgListOpen) >>
			lit(ArgListClose)
			)
			)
			[
				phx::nothing
			];

		speed_control_ =
			lit("speed") >>
			lit(ArgListOpen) >>
			(
			-(lit("max_speed") >> lit(ArgListOpen) >> double_ >> lit(ArgListClose))	// optional rather than permutation since only 1 possibility
			) >>
			lit(ArgListClose)
			;

		positional_control_ =
			lit("positional") >>
			lit(ArgListOpen) >>
			(
			(lit("gain") >> lit(ArgListOpen) >> double_ >> lit(ArgListClose))
			^ (lit("max_speed") >> lit(ArgListOpen) >> double_ >> lit(ArgListClose))
			) >>
			lit(ArgListClose)
			;

		control_ =
			lit("control") >>
			lit(ArgListOpen) >>
			(
			torque_control_
			| speed_control_
			| positional_control_
			) >>
			lit(ArgListClose)
			;

		props_ =
			anchors_
			^ limits_parser_
			^ max_torque_
			^ control_
			;

		start_ =
			lit("joint") >>
			-(lit(':') >> name_parser_) >>
			lit(ArgListOpen) >>
			props_ >>
			lit(ArgListClose)
			;
	}

	template struct joint_parser < std::string::const_iterator > ;

}




