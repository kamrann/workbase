// camera_mesh.h

#ifndef __WB_3D_CAMERA_MESH_H
#define __WB_3D_CAMERA_MESH_H

#include "primitives.h"


template < typename float_t >
mesh< float_t > generate_camera_mesh(
	float_t size
	)
{
	typedef mesh< float_t > mesh_t;

	float const PropLength = 1.0f;
	float const PropHeight = 0.4f;
	float const PropWidth = 0.3f;
	float const LensPropSize = PropHeight * 1.2f;
	float const ReelPropSize = PropHeight * 1.0f;
	float const ReelPropWidth = PropWidth / 3;
	float const ReelPropGap = ReelPropWidth;

	mesh_t body_mesh = generate_cuboid_mesh(
		PropWidth * size,
		PropHeight * size,
		PropLength * size,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals
		);

	mesh_t lens_mesh = generate_conical_mesh(
		4,
		LensPropSize / 2 * size,
		LensPropSize * size,
		true,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals
		);
	lens_mesh.transform(
		Eigen::Translation< float, 3 >(0, 0, size * (PropLength / 2 + LensPropSize * 0.6f)) *
		Eigen::AngleAxis< float_t >(boost::math::double_constants::pi, 
			Eigen::Vector3f((float_t)1, (float_t)0, (float_t)0)
			) *
		Eigen::AngleAxis< float_t >(boost::math::double_constants::pi / 4, 
			Eigen::Vector3f((float_t)0, (float_t)0, (float_t)1)
			)
		);

	mesh_t m = body_mesh + lens_mesh;

	mesh_t reel_mesh = generate_cylinder_mesh(
		32,
		ReelPropSize / 2 * size,
		ReelPropWidth,
		true,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals
		);
	reel_mesh.transform(
		Eigen::Translation< float, 3 >(0, size * (PropHeight / 2 + ReelPropSize / 2), size * (ReelPropSize / 2 + ReelPropGap / 2)) *
		Eigen::AngleAxis< float_t >(boost::math::double_constants::pi / 2, 
			Eigen::Vector3f((float_t)0, (float_t)1, (float_t)0)) *
		Eigen::Translation< float, 3 >(0, 0, size * -ReelPropWidth / 2)
		);
	m = m + reel_mesh;
	reel_mesh.transform(
		Eigen::Transform< float, 3, Eigen::Affine >(
			Eigen::Translation< float, 3 >(0, 0, size * -(ReelPropSize + ReelPropGap))
			)
		);
	m = m + reel_mesh;

	return m;
}


#endif

