// param.h

#ifndef __WB_PARAM_H
#define __WB_PARAM_H

#include "util/dimensionality.h"
#include "util/fixed_or_random.h"
#include "util/bimap.h"

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

	extern bimap< ParamType, std::string > const ParamTypeNameMap;


	inline bool is_leaf_type(ParamType t)
	{
		return t < ParamType::_LeafTypeEnd;
	}
	/*
	struct enum_param_val
	{
		boost::any contents;

		enum_param_val(boost::any const& a = boost::any()): contents(a)
		{}
	};
	*/
	typedef std::vector< std::string > enum_param_val;

	template < WorldDimensionality dim >
	struct vec_base
	{
		std::array< double, DimensionalityTraits< dim >::Dimensions > v;

		vec_base() = default;
		vec_base(std::array< double, DimensionalityTraits< dim >::Dimensions > const& vals):
			v{ vals }
		{}

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
		using vec_base< WorldDimensionality::dim2D >::vec_base;

		// TODO: why not being inherited??
		vec(std::array< double, 2 > const& vals):
			vec_base{ vals }
		{}

		vec(std::vector< double > const& vals)
		{
			v[0] = vals[0];
			v[1] = vals[1];
		}

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
		using vec_base< WorldDimensionality::dim3D >::vec_base;

		// TODO: why not being inherited??
		vec(std::array< double, 3 > const& vals):
			vec_base{ vals }
		{}

		vec(std::vector< double > const& vals)
		{
			v[0] = vals[0];
			v[1] = vals[1];
			v[2] = vals[2];
		}

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
//		bool is_fixed;
		typedef boost::variant <
			std::pair < double, double >,
			double
		> variant_t;
		
		variant_t range;

		// TODO: distribution enum

		random(): /*is_fixed(true),*/ range(0.0)
		{}

		random(double d):
			//is_fixed{ true },
			range{ d }
		{}

		random(std::pair< double, double > rg):
			//is_fixed{ false },
			range{ rg }
		{}

		inline bool is_fixed() const
		{
			return range.which() == 1;
		}

		inline double as_fixed() const
		{
			return boost::get< double >(range);
		}

		inline std::pair< double, double > as_range() const
		{
			return boost::get< std::pair< double, double > >(range);
		}
	};


	typedef boost::variant <
		
		bool
		, double
		, int
		, vec2
		, random
		, enum_param_val
		, std::string

	> param;


	template < typename T >
	inline T extract_as(param const& p, T const& default_value = T())
	{
		try
		{
			return boost::get< T >(p);
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
			dest = boost::get< T >(p);
			return true;
		}
		catch(...)
		{
			dest = default_value;
			return false;
		}
	}


//	typedef YAML::Node param;

	/*
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
	*/

	/*
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

	param find_node(
		param const& node,
		qualified_path const& name
		);
		*/
	///////////// TODO: Temp solution
}

//#include <boost/random/mersenne_twister.hpp>
#include <random>

namespace prm {

	fixed_or_random<
		double,
		std::uniform_real_distribution< double >,
		std::default_random_engine
	> extract_fixed_or_random(
		prm::param const& node,
		double default_value = 0.0
		);

}


#endif


