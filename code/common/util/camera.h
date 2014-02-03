// camera.h

#ifndef __WB_CAMERA_H
#define __WB_CAMERA_H

#include "orientation.h"

#include <Eigen/Dense>

#include <boost/math/constants/constants.hpp>
#include <boost/optional.hpp>


class camera
{
public:
	typedef float float_t;
	typedef Eigen::Matrix< float_t, 3, 1 >	vec_t;
	typedef Eigen::Matrix< float_t, 4, 4 >	mat_t;
	typedef Eigen::Matrix< float_t, 3, 3 >	mat3_t;

	typedef Eigen::Quaternion< float_t >						orient_t;
	typedef Eigen::Transform< float_t, 3, Eigen::Affine >		afn_xform_t;
	typedef Eigen::Transform< float_t, 3, Eigen::Projective >	proj_xform_t;

	enum class ProjectionMode {
		Orthographic,
		Perspective
	};

private:
//	proj_xform_t	proj_xf;
//	afn_xform_t		view_xf;
	float_t			aspect;
	float_t			near;
	float_t			far;
	ProjectionMode	mode;
	float_t			zoom_factor;
	orient_t		orientation;
	vec_t			pos;

public:
	camera()
	{
//		proj_xf.setIdentity();
		aspect = 1.0;
		near = 0.01;
		far = 1.0;
		mode = ProjectionMode::Orthographic;
		zoom_factor = 1.0;
		orientation.setIdentity();
		pos.setZero();
	}

private:
	proj_xform_t get_orthographic_projection() const
	{
		float_t right = aspect / zoom_factor;
		float_t left = -right;
		float_t top = 1.0 / zoom_factor;
		float_t bottom = -top;

		mat_t M;
		M <<
			2.0 / (right - left),	0,						0,						-(right + left) / (right - left),
			0,						2.0 / (top - bottom),	0,						-(top + bottom) / (top - bottom),
			0,						0,						/* - */2.0 / (far - near),	-(far + near) / (far - near), 
			0,						0,						0,						1.0
			;
		return proj_xform_t(M);
	}

	proj_xform_t get_perspective_projection() const
	{
		float_t fovY = (boost::math::double_constants::pi / 3.0) / zoom_factor;
		float_t theta = fovY * 0.5;
		float_t range = far - near;
		float_t invtan = 1.0 / std::tan(theta);

		mat_t M;
		M.setIdentity();
		M(0,0) = invtan / aspect;
		M(1,1) = invtan;
		M(2,2) = /* - */(near + far) / range;
		M(3,2) = /* - */1;
		M(2,3) = -2 * near * far / range;
		M(3,3) = 0;
		return proj_xform_t(M);
	}

public:
	inline vec_t get_position() const
	{
		return pos;
	}

	void set_position(vec_t const& p)
	{
		pos = p;
	}

	void set_position(float_t x, float_t y, float_t z)
	{
		pos = vec_t(x, y, z);
	}

	orient_t get_orientation() const
	{
		return orientation;
	}

	void set_orientation(orient_t o)
	{
		orientation = o;
	}

	void move(float_t x, float_t y, float_t z)
	{
		//view_xf = view_xf * Eigen::Translation< float_t, 3 >(-x, -y, -z);
		pos += vec_t(x, y, z);
	}

	void move_local(float_t x, float_t y, float_t z)
	{
		pos += orientation * vec_t(x, y, z);
	}

	void pitch(float_t radians)
	{
		orientation = orientation * Eigen::AngleAxis< float_t >(radians, vec_t((float_t)1, (float_t)0, (float_t)0));
	}

	void yaw(float_t radians)
	{
		orientation = orientation * Eigen::AngleAxis< float_t >(radians, vec_t((float_t)0, (float_t)1, (float_t)0));
	}

	void zoom(float_t relative_factor)
	{
		zoom_factor *= relative_factor;
	}

	void look_at(vec_t const& target, vec_t const& up = vec_t(0.0, 1.0, 0.0))
	{
		orientation = lookat_orientation((target - pos).normalized(), up);

		/*
		mat3_t R;
		R.col(2) = (pos - target).normalized();
		R.col(0) = up.cross(R.col(2)).normalized();
		R.col(1) = R.col(2).cross(R.col(0));

//		mat_t M;
//		M.setIdentity();
//		M.topLeftCorner< 3, 3 >() = R.transpose();
//		M.topRightCorner< 3, 1 >() = -R.transpose() * position;

//		view_xf = M;
		orientation = R.transpose();//M.inverse();
		*/
	}

	void set_aspect(float_t new_aspect)
	{
		aspect = new_aspect;
	}

	float_t get_aspect() const
	{
		return aspect;
	}

	void set_clip_distances(float_t _near, float_t _far)
	{
		near = _near;
		far = _far;
	}

	void set_projection_mode(
		ProjectionMode m,
		boost::optional< float_t > _near = boost::optional< float_t >(),
		boost::optional< float_t > _far = boost::optional< float_t >()
		)
	{
		mode = m;

		if(_near)
		{
			near = *_near;
		}
		if(_far)
		{
			far = *_far;
		}
	}

/*
	inline mat_t const& projection_matrix() const
	{
		return proj_xf.matrix();
	}

	inline mat_t const& view_matrix() const
	{
		return view_xf.matrix();
	}
*/
	inline proj_xform_t proj_xform() const
	{
		switch(mode)
		{
		case ProjectionMode::Orthographic:
			return get_orthographic_projection();
		case ProjectionMode::Perspective:
			return get_perspective_projection();
		}
	}

	inline afn_xform_t view_xform() const
	{
		return orientation.inverse() * Eigen::Translation< float_t, 3 >(-pos);
	}

	inline proj_xform_t vp_xform() const
	{
		return proj_xform() * view_xform();
	}

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


#endif


