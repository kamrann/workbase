// random.h

#ifndef __WB_3D_RANDOM_H
#define __WB_3D_RANDOM_H

#include <Eigen/Dense>

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/math/constants/constants.hpp>


// Generate any perpendicular vector
inline Eigen::Vector3f any_perpendicular(Eigen::Vector3f const& v)
{
	// TODO: Possible precision errors when v[0] is very close to 0, w.r.t. its other components??
	if(v[0] == 0)
	{
		// Cross with (1, 0, 0)
		return Eigen::Vector3f(0, v[2], -v[1]) / sqrt(v[1] * v[1] + v[2] * v[2]);
	}
	else
	{
		// Cross with (0, 1, 0)
		return Eigen::Vector3f(-v[2], 0, v[0]) / sqrt(v[0] * v[0] + v[2] * v[2]);
	}
}

// Generate random perpendicular vector
template < typename RGen >
inline Eigen::Vector3f random_perpendicular(Eigen::Vector3f const& v, RGen& rgen)
{
	boost::random::uniform_real_distribution<> dist(0.0f, 2.0f * boost::math::double_constants::pi);
	float const theta = dist(rgen);
	float const cs = std::cos(theta);
	float const sn = std::sin(theta);
	if(std::abs(v[0]) >= std::abs(v[1]))
	{
		return (
			Eigen::Vector3f(-v[2], 0, v[0]) * cs +
			Eigen::Vector3f(v[0] * v[1], -v[0] * v[0] - v[2] * v[2], v[1] * v[2]) * sn
			) / std::sqrt(v[0] * v[0] + v[2] * v[2]);
	}
	else
	{
		return (
			Eigen::Vector3f(0, v[2], -v[1]) * cs +
			Eigen::Vector3f(-v[1] * v[1] - v[2] * v[2], v[0] * v[1], v[0] * v[2]) * sn
			) / std::sqrt(v[1] * v[1] + v[2] * v[2]);
	}
}

// Generates a vector on a hemisphere normally distributed around the given mean vector.
// The generated vector will be of the same magnitude as the mean vector.
// A std dev of 1 means 68% will be within 45 degrees
// A std dev of 0.5 will give 95% within 45 degrees
// A std dev of 0.33 will give 99.7% within 45 degrees
template < typename RGen >
inline Eigen::Vector3f normally_dist_hemisphere_vec(
	Eigen::Vector3f const& mean_vec,
	RGen& rgen,
	float std_dev = 1.0f
	)
{
	boost::random::normal_distribution<> dist(0.0, std_dev);
	double const tan = dist(rgen);
	double const theta = std::atan(tan);

	Eigen::Vector3f perp = any_perpendicular(mean_vec);
	// TODO: More efficient way...
	float phi = boost::random::uniform_real_distribution<>(0, boost::math::double_constants::pi)(rgen);
	return Eigen::AngleAxis< float >(phi, mean_vec.normalized()) *
		Eigen::AngleAxis< float >(theta, perp) * mean_vec;
}


#endif

