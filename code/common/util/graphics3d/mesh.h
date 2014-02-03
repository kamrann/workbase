// mesh.h

#ifndef __GLES_UTIL_MESH_H
#define __GLES_UTIL_MESH_H

#include "util/enum_flags.h"

#include <Eigen/Dense>

#include <boost/optional.hpp>

#include <vector>


class mesh_base
{
public:
	enum class vtx_elements {
		Positions		= 0x00,
		Normals			= 0x01,
		TexCoords		= 0x02,
		Colors			= 0x04,
	};

	static const size_t		VertexComponents		= 3;
	static const size_t		NormalComponents		= 3;
	static const size_t		TexCoordComponents		= 2;
	static const size_t		ColorComponents			= 4;
};

ENUM_FLAGS(mesh_base::vtx_elements);


template <
	typename FloatType
>
class mesh: public mesh_base
{
public:
	typedef FloatType											float_t;
	typedef Eigen::Matrix< float_t, VertexComponents, 1 >		vertex_t;
	typedef Eigen::Matrix< float_t, NormalComponents, 1 >		normal_t;
	typedef Eigen::Matrix< float_t, TexCoordComponents, 1 >		tex_coord_2d_t;
	typedef Eigen::Matrix< float_t, ColorComponents, 1 >		color_t;
	typedef unsigned short										index_t;

private:
	std::vector< float_t >							vertices;
	boost::optional< std::vector< float_t > >		normals;
	boost::optional< std::vector< float_t > >		tex_coords;
	boost::optional< std::vector< float_t > >		colors;

	std::vector< index_t >							indices;

public:
	mesh()
	{}

	mesh operator+ (mesh const& other) const;

private:
	inline void set_vertex(size_t idx, vertex_t const& v)
	{
		for(size_t c = 0; c < VertexComponents; ++c)
		{
			vertices[idx * VertexComponents + c] = v[c];
		}
	}

	inline void set_normal(size_t idx, normal_t const& n)
	{
		assert(normals);
		for(size_t c = 0; c < NormalComponents; ++c)
		{
			(*normals)[idx * NormalComponents + c] = n[c];
		}
	}

public:
	void transform(Eigen::Transform< float_t, 3, Eigen::Affine > xform);
	void convert_to_wireframe();

public:
	inline bool has_normals() const
	{
		return normals;
	}

	inline bool has_tex_coords() const
	{
		return tex_coords;
	}

	inline bool has_colors() const
	{
		return colors;
	}

	inline size_t num_vertices() const
	{
		return vertices.size() / VertexComponents;
	}

	inline size_t num_indices() const
	{
		return indices.size();
	}

	inline vertex_t get_vertex(size_t const i) const
	{
		return vertex_t(
			vertices[i * VertexComponents + 0],
			vertices[i * VertexComponents + 1],
			vertices[i * VertexComponents + 2]
			);
	}

	inline normal_t get_normal(size_t const i) const
	{
		return normal_t(
			(*normals)[i * NormalComponents + 0],
			(*normals)[i * NormalComponents + 1],
			(*normals)[i * NormalComponents + 2]
			);
	}

	inline tex_coord_2d_t get_tex_coords(size_t const i) const
	{
		return tex_coord_2d_t(
			(*tex_coords)[i * TexCoordComponents + 0],
			(*tex_coords)[i * TexCoordComponents + 1]
			);
	}

	inline color_t get_color(size_t const i) const
	{
		return color_t(
			(*colors)[i * ColorComponents + 0],
			(*colors)[i * ColorComponents + 1],
			(*colors)[i * ColorComponents + 2],
			(*colors)[i * ColorComponents + 3]
			);
	}

	inline index_t get_index(size_t const i) const
	{
		return indices[i];
	}

	inline void clear()
	{
		vertices.clear();
		indices.clear();

		normals.reset();
		tex_coords.reset();
		colors.reset();
	}

	template < typename InputRange >
	inline void set_vertices(InputRange const& verts)
	{
		clear();

		vertices.resize(verts.size() * VertexComponents);
		size_t idx = 0;
		for(vertex_t const& v: verts)
		{
			for(size_t c = 0; c < VertexComponents; ++c)
			{
				vertices[idx++] = v[c];
			}
		}
	}

	template < typename InputRange >
	inline void set_normals(InputRange const& norms)
	{
		normals = std::vector< float_t >();

		normals->resize(norms.size() * NormalComponents);
		size_t idx = 0;
		for(normal_t const& n: norms)
		{
			for(size_t c = 0; c < NormalComponents; ++c)
			{
				(*normals)[idx++] = n[c];
			}
		}
	}

	template < typename InputRange >
	inline void set_tex_coords(InputRange const& coords)
	{
		tex_coords = std::vector< float_t >();

		tex_coords->resize(coords.size() * TexCoordComponents);
		size_t idx = 0;
		for(tex_coord_2d_t const& t: coords)
		{
			for(size_t c = 0; c < TexCoordComponents; ++c)
			{
				(*tex_coords)[idx++] = t[c];
			}
		}
	}

	template < typename InputRange >
	inline void set_colors(InputRange const& cols)
	{
		colors = std::vector< float_t >();

		colors->resize(cols.size() * ColorComponents);
		size_t idx = 0;
		for(color_t const& cl: cols)
		{
			for(size_t c = 0; c < ColorComponents; ++c)
			{
				(*colors)[idx++] = cl[c];
			}
		}
	}

	template < typename InputRange >
	inline void set_indices(InputRange const& inds)
	{
		indices.resize(inds.size());
		std::copy(inds.begin(), inds.end(), indices.begin());
	}

	const float_t* vertex_data() const
	{
		return vertices.data();
	}

	const float_t* normal_data() const
	{
		return normals ? normals->data() : nullptr;
	}

	const float_t* tex_coord_data() const
	{
		return tex_coords ? tex_coords->data() : nullptr;
	}

	const float_t* color_data() const
	{
		return colors ? colors->data() : nullptr;
	}

	const index_t* index_data() const
	{
		return indices.data();
	}
};


template < typename FloatType >
mesh< FloatType > mesh< FloatType >::operator+ (mesh< FloatType > const& other) const
{
	mesh m;

	m.vertices.resize((num_vertices() + other.num_vertices()) * VertexComponents);
	std::copy(vertices.begin(), vertices.end(), m.vertices.begin());
	std::copy(other.vertices.begin(), other.vertices.end(), m.vertices.begin() + num_vertices() * VertexComponents);

	if(has_normals() && other.has_normals())
	{
		m.normals = std::vector< float_t >((num_vertices() + other.num_vertices()) * NormalComponents);
		std::copy(normals->begin(), normals->end(), m.normals->begin());
		std::copy(other.normals->begin(), other.normals->end(), m.normals->begin() + num_vertices() * NormalComponents);
	}

	if(has_tex_coords() && other.has_tex_coords())
	{
		m.tex_coords = std::vector< float_t >((num_vertices() + other.num_vertices()) * TexCoordComponents);
		std::copy(tex_coords->begin(), tex_coords->end(), m.tex_coords->begin());
		std::copy(other.tex_coords->begin(), other.tex_coords->end(), m.tex_coords->begin() + num_vertices() * TexCoordComponents);
	}

	if(has_colors() && other.has_colors())
	{
		m.colors = std::vector< float_t >((num_vertices() + other.num_vertices()) * ColorComponents);
		std::copy(colors->begin(), colors->end(), m.colors->begin());
		std::copy(other.colors->begin(), other.colors->end(), m.colors->begin() + num_vertices() * ColorComponents);
	}

	m.indices.resize(num_indices() + other.num_indices());
	std::copy(indices.begin(), indices.end(), m.indices.begin());
	std::transform(other.indices.begin(), other.indices.end(), m.indices.begin() + num_indices(),
		[this](index_t idx){ return idx + num_vertices(); }
	);

	return m;
}

template < typename FloatType >
void mesh< FloatType >::transform(Eigen::Transform< FloatType, 3, Eigen::Affine > xform)
{
	for(size_t v = 0; v < num_vertices(); ++v)
	{
		set_vertex(v, xform * get_vertex(v));
	}
	
	if(normals)
	{
		// TODO: See EIgen docs, Space Transformations | Affine transformations 
		// It suggests using .linear()/.translation() calls, rather than differentiating point/vector by 4th component 1/0...

		Eigen::Transform< FloatType, 3, Eigen::Affine > norm_xform(xform.matrix().inverse().transpose());
		for(size_t v = 0; v < num_vertices(); ++v)
		{
			Eigen::Vector4f hom_n;
			hom_n.topRows< 3 >() = get_normal(v);
			hom_n[3] = 0.0f;
			set_normal(v, (norm_xform * hom_n).topRows< 3 >().normalized());
		}
	}
}

// TODO: Assuming currently GL_TRIANGLES type mesh
// Also, probably want to have a separate class type for each type, so this method will need to return an object
template < typename FloatType >
void mesh< FloatType >::convert_to_wireframe()
{
	// TODO: For now, just simple implementation resulting in duplicates
	// Will need to decide if want to retain normals for lighting wireframe to determine how can best optimise this
	size_t const num_inds = num_indices();
	std::vector< index_t > new_indices(num_inds * 2);
	size_t j = 0;
	for(size_t i = 0; i < num_inds; i += 3)
	{
		new_indices[j++] = indices[i];
		new_indices[j++] = indices[i + 1];

		new_indices[j++] = indices[i + 1];
		new_indices[j++] = indices[i + 2];

		new_indices[j++] = indices[i + 2];
		new_indices[j++] = indices[i];
	}

	set_indices(new_indices);
}


#endif



