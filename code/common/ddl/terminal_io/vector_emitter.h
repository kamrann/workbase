// vector_emitter.h

#ifndef __WB_PARAMS_VECTOR_EMITTER_H
#define __WB_PARAMS_VECTOR_EMITTER_H

#include "param.h"
#include "value_parsing_ast.h"

#include <boost/spirit/include/karma.hpp>
#include <boost/fusion/adapted/adt.hpp>
#include <boost/fusion/adapted/struct.hpp>


namespace karma = boost::spirit::karma;

namespace ddl {

	template < /*WorldDimensionality Dim,*/ typename OutputIterator >
//	struct vector_emitter: karma::grammar< OutputIterator, vec< Dim >() >
	struct vec2_emitter: karma::grammar< OutputIterator, vec2() >
	{
//		typedef vec< Dim > vector_type;
		typedef vec2 vector_type;
//		enum { Dimensions = DimensionalityTraits< Dim >::Dimensions };

		vec2_emitter(): vec2_emitter::base_type(start)
		{
			using karma::lit;
			using karma::double_;
			
			start %= 
				(
				lit("(") <<
				double_ <<
				lit(", ") <<
				double_ <<
				lit(")")
				)
				;
		}

		karma::rule< OutputIterator, vector_type() > start;
	};

/*	template < typename OutputIterator >
	using vec2_emitter = vector_emitter< WorldDimensionality::dim2D, OutputIterator >;
	
	template < typename OutputIterator >
	using vec3_emitter = vector_emitter< WorldDimensionality::dim3D, OutputIterator >;
*/
}


#endif


