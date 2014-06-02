// param.h

#ifndef __WB_PARAM_H
#define __WB_PARAM_H

#include "util/dimensionality.h"

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/any.hpp>

#include <string>
#include <vector>


namespace prm
{
	enum ParamType {
		// Fundamental types
		Boolean,
		Integer,
		RealNumber,
		String,
		Enumeration,
		Vector2,
//		Vector3,
		
		_LeafTypeEnd,
		__Composite = _LeafTypeEnd - 1,

		// Composite
		List,	// A list of params

		Count,
		None = Count,
	};

	inline bool is_leaf_type(ParamType t)
	{
		return t < ParamType::_LeafTypeEnd;
	}

	struct enum_param_val
	{
		boost::any contents;

		enum_param_val(boost::any const& a = boost::any()): contents(a)
		{}
	};

	template < WorldDimensionality dim >
	struct vec_base
	{
		std::array< double, DimensionalityTraits< dim >::Dimensions > v;

		vec_base() = default;

		// TODO: Eigen construction and cast

		double& operator[] (int i)
		{
			return v[i];
		}
		
		double const& operator[] (int i) const
		{
			return v[i];
		}
	};

	template < WorldDimensionality dim >
	struct vec;

	template <>
	struct vec< WorldDimensionality::dim2D >: public vec_base< WorldDimensionality::dim2D >
	{
		vec() = default;

		vec(double x, double y)
		{
			v[0] = x;
			v[1] = y;
		}
	};

	template <>
	struct vec< WorldDimensionality::dim3D >: public vec_base< WorldDimensionality::dim3D >
	{
		vec() = default;

		vec(double x, double y, double z)
		{
			v[0] = x;
			v[1] = y;
			v[2] = z;
		}
	};

	typedef vec< WorldDimensionality::dim2D > vec2;
	typedef vec< WorldDimensionality::dim3D > vec3;


	typedef boost::make_recursive_variant<
		bool,
		int,
		double,
		std::string,
		enum_param_val,
		vec2,
//		vec3,

		std::vector< boost::recursive_variant_ >

	>::type param;

	typedef std::vector< param > param_list;


	// TODO: Use template?
	inline param create_from_type(ParamType t)
	{
		switch(t)
		{
			case ParamType::Boolean:
			return param(false);

			case ParamType::Integer:
			return param((int)0);

			case ParamType::RealNumber:
			return param(0.0);

			case ParamType::String:
			return param(std::string());

			case ParamType::Enumeration:
			return param(enum_param_val(boost::any()));

			case ParamType::Vector2:
			return param(vec2());

//			case ParamType::Vector3:
//			return param(vec3());

			case ParamType::List:
			return param(std::vector< param >());

			default:
			assert(false);
			return param();
		}
	}
}


#endif


