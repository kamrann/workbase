// object_parser.cpp

#include "object_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::object,
	(boost::optional< std::string >, name)
	(std::list< b2ds::ast::component >, components)
	);

namespace b2ds {

	namespace phx = boost::phoenix;

	template < typename Iterator >
	object_parser< Iterator >::aabb_coord_symbols::aabb_coord_symbols()
	{
		add
			("Center", ast::AABB_Coords::Center)
			("TopLeft", ast::AABB_Coords::TopLeft)
			("TopRight", ast::AABB_Coords::TopRight)
			("BottomLeft", ast::AABB_Coords::BottomLeft)
			("BottomRight", ast::AABB_Coords::BottomRight)
			("LeftCenter", ast::AABB_Coords::LeftCenter)
			("RightCenter", ast::AABB_Coords::RightCenter)
			("TopCenter", ast::AABB_Coords::TopCenter)
			("BottomCenter", ast::AABB_Coords::BottomCenter)
			;
	}

	template < typename Iterator >
	object_parser< Iterator >::object_parser():
		base_type(start_)
	{
		using qi::lit;
		using qi::double_;

		rotation_ =
			lit("rotation") >>
			lit(ArgListOpen) >>
			double_ >>
			lit(ArgListClose)
			;

		base_ =
			lit("base") >>
			lit(ArgListOpen) >>
			(rel_coord2d_parser_ | aabb_coords_parser_) >>
			lit(ArgListClose)
			;

		component_ =
			body_parser_
			| joint_parser_
			| sucker_parser_
			| rotation_
			| base_
			;

		start_ =
			lit("object") >>
			-name_parser_ >>
			lit(ArgListOpen) >>
			*component_ >>
			lit(ArgListClose)
			;
	}

	template struct object_parser < std::string::const_iterator > ;

}




