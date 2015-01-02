// joint_parser.h

#ifndef __WB_B2D_SCRIPT_JOINT_PARSER_H
#define __WB_B2D_SCRIPT_JOINT_PARSER_H

#include "name_parser.h"
#include "relative_coord2d_parser.h"
#include "joint_limits_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		struct joint_anchors
		{
			rel_coord2d b1, b2;
		};
		typedef double max_torque;
		
		struct torque_control {};
		struct speed_control
		{
			boost::optional< double > max_speed;
		};
		struct positional_control
		{
			boost::optional< double > gain;
			boost::optional< double > max_speed;
		};
		typedef boost::variant <
			torque_control
			, speed_control
			, positional_control
		> control;

		struct joint_props
		{
			boost::optional< joint_anchors > anchors;
			boost::optional< joint_limits > limits;
			boost::optional< max_torque > max_torque;
			boost::optional< control > control;
		};

		struct joint
		{
			boost::optional< std::string > name;
			joint_props props;
		};
	}
	

	template < typename Iterator >
	struct joint_parser: qi::grammar< Iterator, ast::joint(), qi::space_type >
	{
		joint_parser();

		qi::rule< Iterator, ast::joint_anchors(), qi::space_type > anchors_;
		qi::rule< Iterator, double(), qi::space_type > max_torque_;
		qi::rule< Iterator, ast::torque_control(), qi::space_type > torque_control_;
		qi::rule< Iterator, ast::speed_control(), qi::space_type > speed_control_;
		qi::rule< Iterator, ast::positional_control(), qi::space_type > positional_control_;
		qi::rule< Iterator, ast::control(), qi::space_type > control_;
		qi::rule< Iterator, ast::joint_props(), qi::space_type > props_;
		qi::rule< Iterator, ast::joint(), qi::space_type > start_;

		name_parser< Iterator > name_parser_;
		relative_coord2d_parser< Iterator > rel_coord_parser_;
		joint_limits_parser< Iterator > limits_parser_;
	};

}


#endif


