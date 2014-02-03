// primitives.h

#ifndef __GLES_UTIL_PRIMITIVES_H
#define __GLES_UTIL_PRIMITIVES_H

#include "mesh.h"

#include <Eigen/StdVector>

#include <boost/math/constants/constants.hpp>
#include <boost/assign/list_of.hpp>


template < typename float_t >
mesh< float_t > generate_cuboid_mesh(
	float_t x_size,
	float_t y_size,
	float_t z_size,
	mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions
	)
{
	typedef mesh< float_t > mesh_t;
	typedef typename mesh_t::vertex_t vertex_t;
	typedef typename mesh_t::normal_t normal_t;
	typedef typename mesh_t::tex_coord_2d_t tex_coord_2d_t;
	typedef typename mesh_t::color_t color_t;
	typedef typename mesh_t::index_t index_t;

	size_t const num_vertices = 24;
	size_t const num_indices = 36;

	float_t const cube_vertices[] = {
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f,  0.5f, 0.5f,
		0.5f,  0.5f, 0.5f, 
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
	};

	float_t const cube_normals[] = {
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	float_t const cube_tex[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};

	std::vector< index_t > cube_indices = boost::assign::list_of
		(0) (2) (1)
		(0) (3) (2) 
		(4) (5) (6)
		(4) (6) (7)
		(8) (9) (10)
		(8) (10) (11) 
		(12) (15) (14)
		(12) (14) (13) 
		(16) (17) (18)
		(16) (18) (19) 
		(20) (23) (22)
		(20) (22) (21);

	vertex_t scale(x_size, y_size, z_size);
	mesh_t m;
	std::vector< vertex_t, Eigen::aligned_allocator< vertex_t > > vertices(num_vertices);
	for(size_t i = 0; i < num_vertices; ++i)
	{
		vertices[i] = vertex_t(
			cube_vertices[i * 3 + 0],
			cube_vertices[i * 3 + 1],
			cube_vertices[i * 3 + 2]
			).cwiseProduct(scale);
	}
	m.set_vertices(vertices);
	
	if(flags(elements & mesh_base::vtx_elements::Normals))
	{
		std::vector< normal_t, Eigen::aligned_allocator< normal_t > > normals(num_vertices);
		for(size_t i = 0; i < num_vertices; ++i)
		{
			normals[i] = normal_t(
				cube_normals[i * 3 + 0],
				cube_normals[i * 3 + 1],
				cube_normals[i * 3 + 2]
				);
		}
		m.set_normals(normals);
	}
	
	if(flags(elements & mesh_base::vtx_elements::TexCoords))
	{
		std::vector< tex_coord_2d_t, Eigen::aligned_allocator< tex_coord_2d_t > > tex_coords(num_vertices);
		for(size_t i = 0; i < num_vertices; ++i)
		{
			tex_coords[i] = tex_coord_2d_t(
				cube_tex[i * 2 + 0],
				cube_tex[i * 2 + 1]
				);
		}
		m.set_tex_coords(tex_coords);
	}
	
	if(flags(elements & mesh_base::vtx_elements::Colors))
	{
		// TODO:
		std::vector< color_t > colors(num_vertices, color_t(1.0f, 1.0f, 1.0f, 1.0f));
		m.set_colors(colors);
	}
	
	m.set_indices(cube_indices);

	return m;
}

template < typename float_t >
mesh< float_t > generate_sphere_mesh(
	size_t num_slices,
	float_t radius,
	mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions)
{
	typedef mesh< float_t > mesh_t;
	typedef typename mesh_t::vertex_t vertex_t;
	typedef typename mesh_t::normal_t normal_t;
	typedef typename mesh_t::tex_coord_2d_t tex_coord_2d_t;
	typedef typename mesh_t::color_t color_t;
	typedef typename mesh_t::index_t index_t;

	size_t const num_parallels = num_slices / 2;
	size_t const num_vertices = (num_parallels + 1) * (num_slices + 1);
	size_t num_indices = num_parallels * num_slices * 6;
	float angle_step = (2.0f * boost::math::double_constants::pi) / num_slices;

	std::vector< vertex_t, Eigen::aligned_allocator< vertex_t > > vertices(num_vertices);
	std::vector< normal_t, Eigen::aligned_allocator< normal_t > > normals(num_vertices);
	std::vector< tex_coord_2d_t, Eigen::aligned_allocator< tex_coord_2d_t > > tex_coords(num_vertices);
	std::vector< color_t, Eigen::aligned_allocator< color_t > > colors(num_vertices, color_t(1.0f, 1.0f, 1.0f, 1.0f));	// TODO: 
	std::vector< index_t > indices(num_indices);

	for(size_t i = 0; i < num_parallels + 1; ++i)
	{
		for(size_t j = 0; j < num_slices + 1; ++j)
		{
			size_t const vertex = i * (num_slices + 1) + j;

			vertices[vertex] = vertex_t(
				std::sin(angle_step * i) * std::sin(angle_step * j),
				std::cos(angle_step * i),
				std::sin(angle_step * i) * std::cos(angle_step * j)
				)
				* radius;

			normals[vertex] = normal_t(vertices[vertex]) / radius;

			tex_coords[vertex] = tex_coord_2d_t(
				(float)j / num_slices,
				(1.0f - (float)i) / (float)(num_parallels - 1)	// TODO: This doesn't look right...
				);

			// TODO: colors
		}
	}

	// Generate the indices
	std::vector< index_t >::iterator it = indices.begin();
	for(size_t i = 0; i < num_parallels; ++i) 
	{
		for(size_t j = 0; j < num_slices; ++j)
		{
			*it++ = i * (num_slices + 1) + j;
			*it++ = (i + 1) * (num_slices + 1) + j;
			*it++ = (i + 1) * (num_slices + 1) + (j + 1);

			*it++ = i * (num_slices + 1) + j;
			*it++ = (i + 1) * (num_slices + 1) + (j + 1);
			*it++ = i * (num_slices + 1) + (j + 1);
		}
	}

	mesh_t m;
	m.set_vertices(vertices);
	if(flags(elements & mesh_base::vtx_elements::Normals))
	{
		m.set_normals(normals);
	}
	if(flags(elements & mesh_base::vtx_elements::TexCoords))
	{
		m.set_tex_coords(tex_coords);
	}
	if(flags(elements & mesh_base::vtx_elements::Colors))
	{
		m.set_colors(colors);
	}
	m.set_indices(indices);

	return m;
}

template < typename float_t >
mesh< float_t > generate_cylinder_mesh(
	size_t num_faces,
	float_t radius,
	float_t length,
	bool closed = false,
	mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions,
	Eigen::Vector4f const& z1_color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),		// Starting lengthwise color value
	Eigen::Vector4f const& z2_color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),		// Ending lengthwise color value
	Eigen::Vector4f const& xy1_color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),		// Starting wraparound color value
	Eigen::Vector4f const& xy2_color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)		// Ending wraparound color value
	)
{
	typedef mesh< float_t > mesh_t;
	typedef typename mesh_t::vertex_t vertex_t;
	typedef typename mesh_t::normal_t normal_t;
	typedef typename mesh_t::tex_coord_2d_t tex_coord_2d_t;
	typedef typename mesh_t::color_t color_t;
	typedef typename mesh_t::index_t index_t;

	size_t const num_vertices = (num_faces + 1) * 2 + (closed ? ((num_faces + 1) * 2 + 2) : 0);
	size_t num_indices = num_faces * 6 + (closed ? num_faces * 6 : 0);
	float angle_step = (2.0f * boost::math::double_constants::pi) / num_faces;

	std::vector< vertex_t, Eigen::aligned_allocator< vertex_t > > vertices(num_vertices);
	std::vector< normal_t, Eigen::aligned_allocator< normal_t > > normals(num_vertices);
	std::vector< tex_coord_2d_t, Eigen::aligned_allocator< tex_coord_2d_t > > tex_coords(num_vertices);
	std::vector< color_t, Eigen::aligned_allocator< color_t > > colors(num_vertices);
	std::vector< index_t > indices(num_indices);

	for(size_t i = 0; i < num_faces + 1; ++i)
	{
		for(size_t j = 0; j < 2; ++j)
		{
			size_t const vertex = i * 2 + j; 

//			if(vertices)
			{
				vertices[vertex] = vertex_t(
					radius * std::sin(angle_step * i),
					radius * std::cos(angle_step * i),
					length * j
					);
			}

//			if(normals)
			{
				normals[vertex] = normal_t(vertices[vertex - j]) / radius;
			}

//			if(texCoords)
			{
				tex_coords[vertex] = tex_coord_2d_t((float)j, (float)i / num_faces);
			}

			// TODO: perhaps pass in functors (with defaults available) to assign each element type
			{
				Eigen::Vector4f lw_val = z1_color + (z2_color - z1_color) * j;
				Eigen::Vector4f wa_val = xy2_color + (xy1_color - xy2_color) * 2.0f * std::abs(0.5f - (float)i / num_faces);
				colors[vertex] = lw_val.cwiseProduct(wa_val);	// TODO: Is this what we want?
			}
		}
	}

	if(closed)
	{
		size_t const base_vertex = (num_faces + 1) * 2;
		
		for(size_t j = 0; j < 2; ++j)
		{
			for(size_t i = 0; i < num_faces + 1; ++i)
			{
				size_t const vertex = base_vertex + (num_faces + 1) * j + i; 

				vertices[vertex] = vertices[i * 2 + j];

				normals[vertex].setZero();
				normals[vertex][2] = (j == 0 ? float_t(-1) : float_t(1));

				// TODO:
				tex_coords[vertex].setZero();
				colors[vertex].setZero();
			}
		}

		// Add the end center vertices
		size_t vertex = (num_faces + 1) * 2 * 2;
		
		vertices[vertex].setZero();
		normals[vertex] = normal_t(float_t(0), float_t(0), float_t(-1));
		// TODO:
		tex_coords[vertex].setZero();
		colors[vertex].setZero();

		++vertex;
		vertices[vertex] = vertex_t(float_t(0), float_t(0), float_t(length));
		normals[vertex] = normal_t(float_t(0), float_t(0), float_t(1));
		// TODO:
		tex_coords[vertex].setZero();
		colors[vertex] = color_t(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// Generate the indices
//	if(indices)
	{
		std::vector< index_t >::iterator it = indices.begin();
		for(size_t i = 0; i < num_faces; ++i) 
		{
			*it++ = i * 2;
			*it++ = i * 2 + 1;
			*it++ = (i + 1) * 2;

			*it++ = (i + 1) * 2;
			*it++ = i * 2 + 1;
			*it++ = (i + 1) * 2 + 1;
		}

		if(closed)
		{
			for(size_t j = 0; j < 2; ++j)
			{
				size_t const vertex_base = (num_faces + 1) * (2 + j);
				size_t const center = (num_faces + 1) * 2 * 2 + j;
				for(size_t i = 0; i < num_faces; ++i) 
				{
					*it++ = center;
					*it++ = vertex_base + i + j;
					*it++ = vertex_base + i + (1 - j);
				}
			}
		}
	}

	mesh_t m;
	m.set_vertices(vertices);
	if(flags(elements & mesh_base::vtx_elements::Normals))
	{
		m.set_normals(normals);
	}
	if(flags(elements & mesh_base::vtx_elements::TexCoords))
	{
		m.set_tex_coords(tex_coords);
	}
	if(flags(elements & mesh_base::vtx_elements::Colors))
	{
		m.set_colors(colors);
	}
	m.set_indices(indices);

	return m;
}

template < typename float_t >
mesh< float_t > generate_conical_mesh(
	size_t num_faces,
	float_t radius,
	float_t length,
	bool closed = false,
	mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions,
	typename mesh< float_t >::color_t base_cl = typename mesh< float_t >::color_t((float_t)1, (float_t)1, (float_t)1, (float_t)1),
	typename mesh< float_t >::color_t point_cl = typename mesh< float_t >::color_t((float_t)1, (float_t)1, (float_t)1, (float_t)1)
	)
{
	typedef mesh< float_t > mesh_t;
	typedef typename mesh_t::vertex_t vertex_t;
	typedef typename mesh_t::normal_t normal_t;
	typedef typename mesh_t::tex_coord_2d_t tex_coord_2d_t;
	typedef typename mesh_t::color_t color_t;
	typedef typename mesh_t::index_t index_t;

	size_t const num_vertices = (num_faces + 1) * 2 + (closed ? ((num_faces + 1) + 1) : 0);
	size_t num_indices = num_faces * 6 + (closed ? num_faces * 3 : 0);
	float angle_step = (2.0f * boost::math::double_constants::pi) / num_faces;

	std::vector< vertex_t, Eigen::aligned_allocator< vertex_t > > vertices(num_vertices);
	std::vector< normal_t, Eigen::aligned_allocator< normal_t > > normals(num_vertices);
	std::vector< tex_coord_2d_t, Eigen::aligned_allocator< tex_coord_2d_t > > tex_coords(num_vertices);
	std::vector< color_t, Eigen::aligned_allocator< color_t > > colors(num_vertices);
	std::vector< index_t > indices(num_indices);

	for(size_t i = 0; i < num_faces + 1; ++i)
	{
		size_t const vertex = i * 2; 

		vertices[vertex] = vertex_t(
			radius * std::sin(angle_step * i),
			radius * std::cos(angle_step * i),
			(float_t)0
			);
		vertices[vertex + 1] = vertex_t(
			(float_t)0,
			(float_t)0,
			length
			);
		
		normals[vertex] = normal_t(vertices[vertex]);
		normals[vertex][2] = radius * radius / length;
		normals[vertex].normalize();
		normals[vertex + 1] = normals[vertex];

		// TODO:
		tex_coords[vertex] = tex_coord_2d_t((float_t)0, (float_t)0);
		tex_coords[vertex + 1] = tex_coord_2d_t((float_t)0, (float_t)0);

		colors[vertex] = base_cl;
		colors[vertex + 1] = point_cl;
	}

	if(closed)
	{
		size_t const base_vertex = (num_faces + 1) * 2;
		
		for(size_t i = 0; i < num_faces + 1; ++i)
		{
			size_t const vertex = base_vertex + i; 

			vertices[vertex] = vertices[i * 2];

			normals[vertex] = normal_t(float_t(0), float_t(0), float_t(-1));

			// TODO:
			tex_coords[vertex].setZero();
			
			colors[vertex] = base_cl;
		}

		// Add the end center vertex
		size_t vertex = (num_faces + 1) * 3;
		
		vertices[vertex].setZero();
		normals[vertex] = normal_t(float_t(0), float_t(0), float_t(-1));
		// TODO:
		tex_coords[vertex].setZero();
		colors[vertex] = base_cl;
	}

	// Generate the indices
	std::vector< index_t >::iterator it = indices.begin();
	for(size_t i = 0; i < num_faces; ++i) 
	{
		*it++ = i * 2;
		*it++ = i * 2 + 1;
		*it++ = (i + 1) * 2;

		*it++ = (i + 1) * 2;
		*it++ = i * 2 + 1;
		*it++ = (i + 1) * 2 + 1;
	}

	if(closed)
	{
		size_t const vertex_base = (num_faces + 1) * 2;
		size_t const center = (num_faces + 1) * 3;
		for(size_t i = 0; i < num_faces; ++i) 
		{
			*it++ = center;
			*it++ = vertex_base + i;
			*it++ = vertex_base + i + 1;
		}
	}

	mesh_t m;
	m.set_vertices(vertices);
	if(flags(elements & mesh_base::vtx_elements::Normals))
	{
		m.set_normals(normals);
	}
	if(flags(elements & mesh_base::vtx_elements::TexCoords))
	{
		m.set_tex_coords(tex_coords);
	}
	if(flags(elements & mesh_base::vtx_elements::Colors))
	{
		m.set_colors(colors);
	}
	m.set_indices(indices);

	return m;
}

/*
Generates a torus around the z axis
*/
template < typename float_t >
mesh< float_t > generate_torus_mesh(
	size_t num_segments,		// Number of section divisions around the base ellipse
	size_t num_faces,			// Number of faces around the tube
	float_t a1, float_t b1,		// x and y semi-axes of the base ellipse
	float_t a2, float_t b2,		// z and x/y semi-axes of the cross sectional ellipse
	mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions)
{
	typedef mesh< float_t > mesh_t;
	typedef typename mesh_t::vertex_t vertex_t;
	typedef typename mesh_t::normal_t normal_t;
	typedef typename mesh_t::tex_coord_2d_t tex_coord_2d_t;
	typedef typename mesh_t::color_t color_t;
	typedef typename mesh_t::index_t index_t;

	typedef Eigen::Matrix< float_t, 3, 1 > vector_t;

	size_t const num_vertices = (num_segments + 1) * (num_faces + 1);
	size_t const num_indices = num_segments * num_faces * 6;

	std::vector< vertex_t, Eigen::aligned_allocator< vertex_t > > vertices(num_vertices);
	std::vector< normal_t, Eigen::aligned_allocator< normal_t > > normals(num_vertices);
	std::vector< tex_coord_2d_t, Eigen::aligned_allocator< tex_coord_2d_t > > tex_coords(num_vertices);
	std::vector< color_t, Eigen::aligned_allocator< color_t > > colors(num_vertices);
	std::vector< index_t > indices(num_indices);

	float const angle_step_1 = (2.0f * boost::math::double_constants::pi) / num_segments;
	float const angle_step_2 = (2.0f * boost::math::double_constants::pi) / num_faces;

	for(size_t i = 0; i < num_segments + 1; ++i)
	{
		// Calculate point on base ellipse
		float_t theta = angle_step_1 * i;
		float_t tan_theta = std::tan(theta);
		float_t Px = a1 * b1 / std::sqrt(b1 * b1 + a1 * a1 * tan_theta * tan_theta);
		Px = boost::math::copysign(Px, std::cos(theta));
		vector_t P(
			Px,
			Px * tan_theta,
			0.0f
			);
		vector_t tangent_at_P(
			-P[1] / (b1 * b1),
			P[0] / (a1 * a1),
			0.0f
			);
		tangent_at_P.normalize();
		vector_t normal_at_P = tangent_at_P.cross(vector_t(0.0f, 0.0f, 1.0f));

		for(size_t j = 0; j < num_faces + 1; ++j)
		{
			size_t const vertex = i * (num_faces + 1) + j;

			float_t gamma = angle_step_2 * j;
			float_t tan_gamma = std::tan(gamma);
			float_t Qx = a2 * b2 / std::sqrt(b2 * b2 + a2 * a2 * tan_gamma * tan_gamma);
			Qx = boost::math::copysign(Qx, std::cos(gamma));
			vector_t Q2d(
				Qx,
				Qx * tan_gamma,
				0.0f
				);
			vector_t tangent_at_Q2d(
				-Q2d[1] / (b2 * b2),
				Q2d[0] / (a2 * a2),
				0.0f
				);

			vector_t Q = P;
			Q += normal_at_P * Q2d[1];
			Q[2] += Q2d[0];

			vector_t tangent_at_Q(0.0f, 0.0f, 0.0f);
			tangent_at_Q += normal_at_P * tangent_at_Q2d[1];
			tangent_at_Q[2] = tangent_at_Q2d[0];
			tangent_at_Q.normalize();
			vector_t normal_at_Q = tangent_at_Q.cross(tangent_at_P);

			vertices[vertex] = Q;
			normals[vertex] = normal_at_Q;
			tex_coords[vertex] = tex_coord_2d_t(0.0f, 0.0f);	// TODO:
			colors[vertex] = color_t((float_t)1, (float_t)1, (float_t)1, (float_t)1);	// TODO:
		}
	}

	// Generate the indices
	std::vector< index_t >::iterator it = indices.begin();
	for(size_t i = 0; i < num_segments; ++i) 
	{
		for(size_t j = 0; j < num_faces; ++j)
		{
			*it++ = i * (num_faces + 1) + j;
			*it++ = (i + 1) * (num_faces + 1) + (j + 1);
			*it++ = (i + 1) * (num_faces + 1) + j;

			*it++ = i * (num_faces + 1) + j;
			*it++ = i * (num_faces + 1) + (j + 1);
			*it++ = (i + 1) * (num_faces + 1) + (j + 1);
		}
	}

	mesh_t m;
	m.set_vertices(vertices);
	if(flags(elements & mesh_base::vtx_elements::Normals))
	{
		m.set_normals(normals);
	}
	if(flags(elements & mesh_base::vtx_elements::TexCoords))
	{
		m.set_tex_coords(tex_coords);
	}
	if(flags(elements & mesh_base::vtx_elements::Colors))
	{
		m.set_colors(colors);
	}
	m.set_indices(indices);

	return m;
}



#endif



