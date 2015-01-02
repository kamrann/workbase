// object_parser.h

#ifndef __WB_B2D_SCRIPT_OBJECT_PARSER_H
#define __WB_B2D_SCRIPT_OBJECT_PARSER_H

#include "name_parser.h"
#include "body_parser.h"
#include "joint_parser.h"
#include "sucker_parser.h"
#include "relative_coord2d_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		enum class AABB_Coords {
			Center,
			TopLeft,
			TopRight,
			BottomLeft,
			BottomRight,
			LeftCenter,
			RightCenter,
			TopCenter,
			BottomCenter,
		};

		typedef boost::variant <
			rel_coord2d
			, AABB_Coords
		> base_coords;

		typedef double obj_rotation;
		typedef base_coords obj_base;

		typedef boost::variant <
			body
			, joint
			, sucker
			, obj_rotation
			, obj_base
		> component;

		struct object
		{
			boost::optional< std::string > name;
			std::list< component > components;
		};
	}

	template < typename Iterator >
	struct object_parser: qi::grammar< Iterator, ast::object(), qi::space_type >
	{
		object_parser();

		qi::rule< Iterator, ast::obj_rotation(), qi::space_type > rotation_;
		qi::rule< Iterator, ast::obj_base(), qi::space_type > base_;
		qi::rule< Iterator, ast::component(), qi::space_type > component_;
		qi::rule< Iterator, ast::object(), qi::space_type > start_;

		name_parser< Iterator > name_parser_;
		body_parser< Iterator > body_parser_;
		joint_parser< Iterator > joint_parser_;
		sucker_parser< Iterator > sucker_parser_;
		relative_coord2d_parser< Iterator > rel_coord2d_parser_;

		struct aabb_coord_symbols: public qi::symbols < char, ast::AABB_Coords >
		{
			aabb_coord_symbols();
		};

		aabb_coord_symbols aabb_coords_parser_;
	};

}


#endif


