// polycode_util.h

#ifndef __POLYCODE_UTIL_H
#define __POLYCODE_UTIL_H

#include "util/dimensionality.h"

#include <Polycode.h>

#include <Eigen/Dense>


inline Polycode::Vector3 to_polycode_vector(Eigen::Vector2d const& v)
{
	return Polycode::Vector3(v[0], v[1], 0.0);
}

inline Polycode::Vector3 to_polycode_vector(Eigen::Vector3d const& v)
{
	return Polycode::Vector3(v[0], v[1], v[2]);
}


#endif


