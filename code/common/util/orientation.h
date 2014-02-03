// orientation.h

#ifndef __WB_ORIENTATION_H
#define __WB_ORIENTATION_H

#include <Eigen/Dense>


inline Eigen::Quaternionf base_orientation(Eigen::Vector3f const& z, Eigen::Vector3f const& y)
{
	Eigen::Matrix< float, 3, 3 > _m;
	_m.col(2) = z;
	_m.col(1) = y;
	_m.col(0) = y.cross(z);
	return Eigen::Quaternionf(_m);
}

inline Eigen::Quaternionf lookat_orientation(Eigen::Vector3f const& look, Eigen::Vector3f const& up = Eigen::Vector3f(0, 1, 0))
{
	Eigen::Matrix< float, 3, 3 > _m;
	_m.col(2) = look;
	_m.col(0) = up.cross(look);
	_m.col(1) = look.cross(_m.col(0));
	return Eigen::Quaternionf(_m);
}


#endif


