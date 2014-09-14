// param.h

#ifndef __WB_PARAM_H
#define __WB_PARAM_H

#include "util/dimensionality.h"
#include "util/fixed_or_random.h"

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/any.hpp>

#include <string>
#include <vector>
#include <set>


namespace YAML {
	class Node;
}

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
		Random,
		
		_LeafTypeEnd,
		__Composite = _LeafTypeEnd - 1,

		// Composite
		List,	// A list of params
		Repeat,

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

	struct random
	{
		bool is_fixed;
		boost::variant<
			double,
			std::pair< double, double >
		> range;

		// TODO: distribution enum

		random(): is_fixed(true), range(0.0)
		{}
	};


	typedef YAML::Node param;


	template < typename T >
	inline T extract_as(param const& p, T const& default_value = T())
	{
		try
		{
			return p.as< T >();
		}
		catch(...)
		{
			return default_value;
		}
	}

	template < typename T >
	inline bool extract_to(param const& p, T& dest, T const& default_value = T())
	{
		try
		{
			dest = p.as< T >();
			return true;
		}
		catch(...)
		{
			dest = default_value;
			return false;
		}
	}


/*	enum ParamNode {
		Name,
		Type,
		Value,
	};
*/
	struct ParamNode
	{
		static const int Name = 0;
		static const int Type = 1;
		static const int Value = 2;
	};

	bool is_param_node(YAML::Node n);

	class qualified_path;

	param instantiate_includes(
		param const& node,
		bool recursive
		);

	std::set< qualified_path > construct_abs_paths(
		param const& p,
		qualified_path const& cur_path
		);
		
	qualified_path resolve_id(
		std::string const& name,
		param const& p,
		qualified_path const& path,
		std::set< qualified_path > const& all_paths
		);

	param find_value(
		param const& node,
		qualified_path const& name
		);

	///////////// TODO: Temp solution
}
#include <boost/random/mersenne_twister.hpp>
namespace prm {

	fixed_or_random<
		double,
		boost::random::uniform_real_distribution< double >,
		boost::random::mt19937
	> extract_fixed_or_random(
		prm::param const& node,
		double default_value = 0.0
		);
	///////////////////

	/*
	typedef boost::make_recursive_variant<
		bool,
		int,
		double,
		std::string,
		enum_param_val,
		vec2,
//		vec3,
		random,

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

			case ParamType::Random:
			return param(random());

			case ParamType::List:
			return param(std::vector< param >());

			default:
			assert(false);
			return param();
		}
	}
	*/
}


#endif


