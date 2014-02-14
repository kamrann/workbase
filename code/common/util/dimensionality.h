// dimensionality.h

#ifndef __DIMENSIONALITY_H
#define __DIMENSIONALITY_H

#include <Eigen/Dense>

#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_real_distribution.hpp>


template < typename T >
struct num_components;

template <>
struct num_components< double >
{
	enum { val = 1 };
};

template <>
struct num_components< Eigen::Vector2d >
{
	enum { val = 2 };
};

template <>
struct num_components< Eigen::Vector3d >
{
	enum { val = 3 };
};


template < typename OutputIterator >
inline size_t get_components(double x, OutputIterator out)
{
	*out++ = x;
	return 1;
}

template < typename OutputIterator >
inline size_t get_components(Eigen::Vector2d const& v, OutputIterator out)
{
	*out++ = v[0];
	*out++ = v[1];
	return 2;
}

template < typename OutputIterator >
inline size_t get_components(Eigen::Vector3d const& v, OutputIterator out)
{
	*out++ = v[0];
	*out++ = v[1];
	*out++ = v[2];
	return 3;
}


template < typename T >
inline T zero_val();

template <>
inline Eigen::Vector2d zero_val< Eigen::Vector2d >()
{
	return Eigen::Vector2d::Zero();
};

template <>
inline Eigen::Vector3d zero_val< Eigen::Vector3d >()
{
	return Eigen::Vector3d::Zero();
};

template <>
inline double zero_val< double >()
{
	return 0.0;
};


inline double magnitude(Eigen::Vector2d const& x)
{
	return x.norm();
}

inline double magnitude(Eigen::Vector3d const& x)
{
	return x.norm();
}

inline double magnitude(double const& x)
{
	return std::abs(x);
}


namespace detail
{
	template < typename T >
	class random_dim
	{};

	template <>
	class random_dim< double >
	{
	public:
		template < typename RGen >
		static inline double dir_val(RGen& rgen)
		{
			return boost::random::uniform_int_distribution<>(0, 1)(rgen) == 0 ? -1.0 : 1.0;
		}
	};

	template <>
	class random_dim< Eigen::Vector2d >
	{
	public:
		template < typename RGen >
		static inline Eigen::Vector2d dir_val(RGen& rgen)
		{
			double theta = boost::random::uniform_real_distribution<>(0.0, 2.0 * boost::math::double_constants::pi)(rgen);
			return Eigen::Vector2d(std::cos(theta), std::sin(theta));
		}
	};

	template <>
	class random_dim< Eigen::Vector3d >
	{
	public:
		template < typename RGen >
		static inline Eigen::Vector3d dir_val(RGen& rgen)
		{
			// TODO:
			return Eigen::Vector3d(1.0, 0, 0);
		}
	};
}


template < typename T, typename RGen >
inline T random_dir_val(RGen& rgen)
{
	return detail::random_dim< T >::dir_val(rgen);
};

template < typename T, typename RGen >
inline T random_val(RGen& rgen)
{
	return random_dir_val< T, RGen >(rgen) * boost::random::uniform_01<>()(rgen);
}


enum class WorldDimensionality {
	dim2D = 2,
	dim3D = 3,
};

template < WorldDimensionality dim >
class DimensionalityTraits
{};

template <>
class DimensionalityTraits< WorldDimensionality::dim2D >
{
public:
	enum { Dimensions = 2 };

	// TODO: necessary to differentiate between position and direction vectors??
	typedef Eigen::Vector2d		position_t;
	typedef Eigen::Vector2d		direction_t;
	typedef direction_t			velocity_t;
	typedef direction_t			force_t;

/*	struct scalar
	{
		double val;

		scalar()
		{}

		scalar(double d): val(d)
		{}

		inline operator double&()
		{
			return val;
		}

		static inline double Zero()
		{
			return 0.0;
		}

		inline void setZero()
		{
			val = 0.0;
		}
	};
*/
	typedef double/*scalar*/	orientation_t;
	typedef double/*scalar*/	angular_direction_t;
	typedef angular_direction_t	angular_velocity_t;
	typedef angular_direction_t	torque_t;

public:
	static inline torque_t torque(position_t const& r, force_t const& f)
	{
		return r[0] * f[1] - r[1] * f[0];
	}

	static inline direction_t transform_dir(direction_t const& v, orientation_t xf)
	{
		return Eigen::Rotation2D< double >(xf) * v;
	}

	static inline direction_t inv_transform_dir(direction_t const& v, orientation_t xf)
	{
		return transform_dir(v, -xf);
	}

	static inline angular_direction_t transform_dir(angular_direction_t const& v, orientation_t xf)
	{
		// In 2D, angular direction does not need transforming between different spaces.
		return v;
	}

	static inline angular_direction_t inv_transform_dir(angular_direction_t const& v, orientation_t xf)
	{
		return transform_dir(v, -xf);
	}

	static inline orientation_t apply_orientation_delta(orientation_t const& orient, angular_direction_t const& delta)
	{
		return orient + delta;
	}
};

template <>
class DimensionalityTraits< WorldDimensionality::dim3D >
{
public:
	enum { Dimensions = 3 };

	typedef Eigen::Vector3d		position_t;
	typedef Eigen::Vector3d		direction_t;
	typedef direction_t			velocity_t;
	typedef direction_t			force_t;

	typedef Eigen::Quaternion< double >	orientation_t;
	typedef Eigen::Vector3d				angular_direction_t;
	typedef angular_direction_t			angular_velocity_t;
	typedef angular_direction_t			torque_t;

public:
	static inline torque_t torque(position_t const& r, force_t const& f)
	{
		return r.cross(f);
	}

	static inline direction_t transform_dir(direction_t const& v, orientation_t const& xf)
	{
		return xf * v;
	}

	static inline direction_t inv_transform_dir(direction_t const& v, orientation_t const& xf)
	{
		return xf.inverse() * v;
	}

	static inline orientation_t apply_orientation_delta(orientation_t const& orient, angular_direction_t const& delta)
	{
		// Probably inefficient, see: http://stackoverflow.com/questions/8816785/looking-for-a-better-method-to-do-quaternion-differentiaton
		double angle = delta.norm();
		return Eigen::AngleAxis< double >(angle, delta / angle) * orient;
	}
};


#endif


