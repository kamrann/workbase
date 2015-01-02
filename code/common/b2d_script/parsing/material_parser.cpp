// material_parser.cpp

#include "material_parser.h"
#include "parsing_common.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::material::properties,
	(boost::optional< b2ds::ast::material::density >, density)
	(boost::optional< b2ds::ast::material::friction >, friction)
	(boost::optional< b2ds::ast::material::restitution >, restitution)
	);

BOOST_FUSION_ADAPT_STRUCT(
	b2ds::ast::material,
	(b2ds::ast::material::Type, type)
	(boost::optional< b2ds::ast::material::properties >, props)
	);

namespace b2ds {

	template < typename Iterator >
	material_properties_parser< Iterator >::material_properties_parser():
		base_type(start_)
	{
		using qi::lit;
		using qi::double_;

		start_ =
			lit(ArgListOpen) >>
			(
			(lit("density") >> lit(ArgListOpen) >> double_ >> lit(ArgListClose))
			^ (lit("friction") >> lit(ArgListOpen) >> double_ >> lit(ArgListClose))
			^ (lit("restitution") >> lit(ArgListOpen) >> double_ >> lit(ArgListClose))
			) >>
			lit(ArgListClose)
			;
	}

	template < typename Iterator >
	material_parser< Iterator >::type_symbols::type_symbols()
	{
		add
			("metal", ast::material::Type::Metal)
			("wood", ast::material::Type::Wood)
			;
	}

	template < typename Iterator >
	material_parser< Iterator >::material_parser():
		base_type(start_)
	{
		using qi::lit;
		using qi::uint_;

		start_ =
			lit("material") >>
			lit(ArgListOpen) >>
			mat_type_ >>
			-properties_parser_ >>
			lit(ArgListClose)
			;
	}

	template struct material_properties_parser < std::string::const_iterator >;
	template struct material_parser < std::string::const_iterator >;

}




