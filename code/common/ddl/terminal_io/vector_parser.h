// vector_parser.h

#ifndef __WB_PARAMS_VECTOR_PARSER_H
#define __WB_PARAMS_VECTOR_PARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	ddl::vec2,
	(double, v[0])
	(double, v[1])
	);

namespace qi = boost::spirit::qi;

namespace ddl {

	template < /*WorldDimensionality Dim,*/ typename Iterator >
//	struct vector_parser: qi::grammar< Iterator, vec< Dim >(), qi::space_type >
	struct vec2_parser
	{
		typedef vec2 vector_type;

		static const char open_ch = '(';
		static const char close_ch = ')';

		struct core: qi::grammar < Iterator, vector_type(), qi::space_type >
		{
			//typedef vec< Dim > vector_type;
			//		enum { Dimensions = DimensionalityTraits< Dim >::Dimensions };

			core(): core::base_type(start)
			{
				using qi::lit;
				using qi::double_;

				start %=
					double_ >>
					lit(",") >>
					double_
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

		struct strict: qi::grammar < Iterator, vector_type(), qi::space_type >
		{
			strict(): strict::base_type(start)
			{
				using qi::lit;

				start =
					lit(open_ch) >>
					cr >>
					lit(close_ch);
			}

			qi::rule< Iterator, vector_type(), qi::space_type > start;

			core cr;
		};

		struct lax: qi::grammar < Iterator, vector_type(), qi::space_type >
		{
			lax(): lax::base_type(start)
			{
				start =
					str
					| cr
					;
			}

			qi::rule< Iterator, vector_type(), qi::space_type > start;

			strict str;
			core cr;
		};
	};

}


#endif


