// joint_limits_parser.h

#ifndef __WB_B2D_SCRIPT_JOINT_LIMITS_PARSER_H
#define __WB_B2D_SCRIPT_JOINT_LIMITS_PARSER_H

#include "b2d_components/components/rotation_joint.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
/*		struct joint_limits
		{
			double lower;
			double upper;
		};
*/
		typedef b2dc::rotation_joint::limits_t joint_limits;
	}

	template < typename Iterator >
	struct joint_limits_parser: qi::grammar< Iterator, ast::joint_limits(), qi::space_type >
	{
		joint_limits_parser();

		qi::rule< Iterator, ast::joint_limits(), qi::space_type > start_;
	};

}


#endif


