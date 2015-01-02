// value_parsing_ast.h

#ifndef __WB_PARAMS_VALUE_PARSING_AST_H
#define __WB_PARAMS_VALUE_PARSING_AST_H

#include "param.h"

#include <boost/variant.hpp>
#include <boost/fusion/adapted/struct.hpp>

#include <string>
#include <set>


BOOST_FUSION_ADAPT_STRUCT(
	prm::vec2,
	(double, v[0])
	(double, v[1])
	);
/*
BOOST_FUSION_ADAPT_STRUCT(
	prm::random,
	(prm::random::variant_t, range)
	);
*/

namespace prm {
#if 0
	typedef boost::variant <
		bool
		, int
		, double
		, vec2
		, random
		, std::vector< std::string >
		, std::string
	> as_parsed;

	struct parsed_value
	{
		ParamType type;
		param val;

		class visitor: public boost::static_visitor < void >
		{
		public:
			visitor(parsed_value& _pv): pv{ _pv }
			{}

		public:
			inline result_type operator() (bool b)
			{
				pv.type = ParamType::Boolean;
				pv.val = b;
			}

			inline result_type operator() (int i)
			{
				pv.type = ParamType::Integer;
				pv.val = i;
			}

			inline result_type operator() (double d)
			{
				pv.type = ParamType::RealNumber;
				pv.val = d;
			}

			inline result_type operator() (vec2 v)
			{
				pv.type = ParamType::Vector2;
				pv.val = v;
			}

			inline result_type operator() (random r)
			{
				pv.type = ParamType::Random;
				pv.val = r;
			}

			inline result_type operator() (enum_param_val const& e)
			{
				pv.type = ParamType::Enumeration;
				pv.val = e;
			}

			inline result_type operator() (std::string str)
			{
				pv.type = ParamType::String;
				pv.val = str;
			}

		private:
			parsed_value& pv;
		};

		parsed_value()
		{}

		parsed_value(as_parsed const& _var)
		{
			visitor vis{ *this };
			boost::apply_visitor(vis, _var);
		}
	};
#endif
}



#endif


