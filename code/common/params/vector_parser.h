// vector_parser.h

#ifndef __WB_PARAMS_VECTOR_PARSER_H
#define __WB_PARAMS_VECTOR_PARSER_H

#include "value_parsing_ast.h"

#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;

namespace prm {

	template < /*WorldDimensionality Dim,*/ typename Iterator >
//	struct vector_parser: qi::grammar< Iterator, vec< Dim >(), qi::space_type >
	struct vec2_parser: qi::grammar< Iterator, vec2(), qi::space_type >
	{
		//typedef vec< Dim > vector_type;
		typedef vec2 vector_type;
//		enum { Dimensions = DimensionalityTraits< Dim >::Dimensions };

		vec2_parser(): vec2_parser::base_type(start)
		{
			using qi::lit;
			using qi::double_;
			using qi::repeat;
			using qi::as;

			start %= 
				lit("(") >>
				double_ >>
				lit(",") >>
				double_ >>
				lit(")")
				;

			/*
			start %= as< std::vector< double > >()	// TODO: Don't understand why this is needed, since 
					// exposed attribute type should already be vector< double >
				[
				lit("(") >>
				double_ >>
				repeat(Dimensions - 1)
				[
				lit(",") >>
				double_
				] >>
				lit(")")
				]
				;
				*/
		}

		qi::rule< Iterator, vector_type(), qi::space_type > start;
	};

/*	template < typename Iterator >
	using vec2_parser = vector_parser< WorldDimensionality::dim2D, Iterator >;
	
	template < typename Iterator >
	using vec3_parser = vector_parser< WorldDimensionality::dim3D, Iterator >;
	*/
}


#endif


