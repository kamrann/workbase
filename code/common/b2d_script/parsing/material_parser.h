// material_parser.h

#ifndef __WB_B2D_SCRIPT_MATERIAL_PARSER_H
#define __WB_B2D_SCRIPT_MATERIAL_PARSER_H

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace b2ds {

	namespace ast {
		struct material
		{
			enum class Type {
				Metal,
				Wood,
			};

			Type type;

			typedef double density;
			typedef double friction;
			typedef double restitution;

/*			typedef std::tuple <
				boost::optional< density >,
				boost::optional< friction >,
				boost::optional < restitution >
			> properties;
*/			
			struct properties
			{
				boost::optional< density > density;
				boost::optional< friction > friction;
				boost::optional< restitution > restitution;

				properties()
				{}
			};

			boost::optional< properties > props;
		};
	}

	template < typename Iterator >
	struct material_properties_parser: qi::grammar< Iterator, ast::material::properties(), qi::space_type >
	{
		material_properties_parser();

		qi::rule< Iterator, ast::material::properties(), qi::space_type > start_;
	};

	template < typename Iterator >
	struct material_parser: qi::grammar< Iterator, ast::material(), qi::space_type >
	{
		material_parser();

		qi::rule< Iterator, ast::material(), qi::space_type > start_;

		struct type_symbols: public qi::symbols < char, ast::material::Type >
		{
			type_symbols();
		};

		type_symbols mat_type_;
		material_properties_parser< Iterator > properties_parser_;
	};

}


#endif


