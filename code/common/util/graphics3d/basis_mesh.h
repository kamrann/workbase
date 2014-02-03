// basis_mesh.h

#ifndef __WB_3D_BASIS_MESH_H
#define __WB_3D_BASIS_MESH_H

#include "primitives.h"


// TODO: Different type, or template parameterized mesh, that has no normals? And maybe indices are automatically interpreted as lines?
template < typename float_t >
mesh< float_t > generate_basis_mesh(
	float_t size,
	Eigen::Vector4f color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)
	//mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions
	)
{
	return generate_basis_mesh(size, color, color, color);
}

template < typename float_t >
mesh< float_t > generate_basis_mesh(
	float_t size,
	Eigen::Vector4f x_cl,
	Eigen::Vector4f y_cl,
	Eigen::Vector4f z_cl
	//mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions
	)
{
	typedef mesh< float_t > mesh_t;
	typedef mesh_t::vertex_t vertex_t;
	typedef mesh_t::color_t color_t;
	typedef mesh_t::index_t index_t;

	std::array< vertex_t, 6 > const vertices = {
		vertex_t(0.0f, 0.0f, 0.0f),
		vertex_t(size, 0.0f, 0.0f),
		vertex_t(0.0f, 0.0f, 0.0f),
		vertex_t(0.0f, size, 0.0f), 
		vertex_t(0.0f, 0.0f, 0.0f),
		vertex_t(0.0f, 0.0f, size)
	};

	std::array< color_t, 6 > const colors = {
		x_cl,
		x_cl,
		y_cl,
		y_cl,
		z_cl,
		z_cl
	};

	std::array< index_t, 6 > const indices = {
		0, 1,
		2, 3,
		4, 5
	};

	mesh< float_t > m;
	m.set_vertices(vertices);
	m.set_colors(colors);
	m.set_indices(indices);

	return m;
}

template < typename float_t >
mesh< float_t > generate_basis_arrows_mesh(
	float_t size,
	Eigen::Vector4f x_arrow_cl = Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
	Eigen::Vector4f y_arrow_cl = Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
	Eigen::Vector4f z_arrow_cl = Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f)
	//mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions
	)
{
	typedef mesh< float_t > mesh_t;
	typedef mesh_t::vertex_t vertex_t;
	typedef mesh_t::color_t color_t;
	typedef mesh_t::index_t index_t;

	float const ArrowPropLength = 0.08f;

	mesh_t x_arrow_mesh = generate_conical_mesh(6, size * ArrowPropLength / 2, size * ArrowPropLength, true,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Colors,
		x_arrow_cl, x_arrow_cl
		);
	x_arrow_mesh.transform(
		Eigen::AngleAxis< float_t >(boost::math::double_constants::pi / 2, 
			Eigen::Vector3f((float_t)0, (float_t)1, (float_t)0)) *
		Eigen::Translation< float_t, 3 >(0, 0, (float_t)size)
		);

	mesh_t y_arrow_mesh = generate_conical_mesh(6, size * ArrowPropLength / 2, size * ArrowPropLength, true,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Colors,
		y_arrow_cl, y_arrow_cl
		);
	y_arrow_mesh.transform(
		Eigen::AngleAxis< float_t >(boost::math::double_constants::pi / 2, 
			Eigen::Vector3f((float_t)-1, (float_t)0, (float_t)0)) *
		Eigen::Translation< float_t, 3 >(0, 0, (float_t)size)
		);

	mesh_t z_arrow_mesh = generate_conical_mesh(6, size * ArrowPropLength / 2, size * ArrowPropLength, true,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Colors,
		z_arrow_cl, z_arrow_cl
		);
	z_arrow_mesh.transform(
		Eigen::Transform< float, 3, Eigen::Affine >(
			Eigen::Translation< float_t, 3 >(0, 0, (float_t)size)
			)
		);

	return x_arrow_mesh + y_arrow_mesh + z_arrow_mesh;
}


#endif

