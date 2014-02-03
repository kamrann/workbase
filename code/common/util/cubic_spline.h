// cubic_spline.h

#ifndef __WB_CUBIC_SPLINE_H
#define __WB_CUBIC_SPLINE_H

#include "graphics3d/mesh.h"
#include "graphics3d/random.h"

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include <boost/math/constants/constants.hpp>

#include <cmath>


struct OneDColorFtr
{
	typedef float float_t;
	typedef Eigen::Matrix< float_t, 4, 1 > color_t;
	color_t a, a_to_b;

	OneDColorFtr(
		color_t const& _a = color_t(1, 1, 1, 1),
		color_t const& _b = color_t(1, 1, 1, 1)
		): a(_a), a_to_b(_b - _a)
	{}

	inline color_t operator() (float_t t, float_t theta) const
	{
		return a + a_to_b * t;
	}
};

struct TwoDTexCoordFtr
{
	typedef float float_t;
	typedef Eigen::Matrix< float_t, 2, 1 > tex_coord_t;
	tex_coord_t a, a_to_b;

	TwoDTexCoordFtr(
		tex_coord_t const& _a = tex_coord_t(0, 0),
		tex_coord_t const& _b = tex_coord_t(1, 1)
		): a(_a), a_to_b(_b - _a)
	{}

	inline tex_coord_t operator() (float_t u, float_t v) const
	{
		return tex_coord_t(
			a[0] + a_to_b[0] * u,
			a[1] + a_to_b[1] * v
			);
	}
};


class cubic_spline_segment
{
public:
	typedef float float_t;
	typedef Eigen::Matrix< float_t, 3, 1 > vec_t;
	typedef Eigen::Matrix< float_t, 4, 4 > coeff_mat_t;
	typedef Eigen::Matrix< float_t, 3, 4 > premult_mat_t;

private:
	premult_mat_t premultiplied;
	vec_t _a, _b, _c, _d, _3a, _2b, _6a;

public:
	cubic_spline_segment()
	{}

//private:
public:
	inline vec_t first_derivative_at(float_t const t) const
	{
		return _3a * t * t + _2b * t + _c;
	}

	inline vec_t second_derivative_at(float_t const t) const
	{
		return _6a * t + _2b;
	}

public:
	inline void set_constraints(
		vec_t p0,
		vec_t p1,
		vec_t v0,
		vec_t v1
		)
	{
		// TODO: Matrix form storage probably not needed
		coeff_mat_t coefficients;
		coefficients <<
			2, -3, 0, 1,
			-2, 3, 0, 0,
			1, -2, 1, 0,
			1, -1, 0, 0
			;

		premultiplied.col(0) = p0;
		premultiplied.col(1) = p1;
		premultiplied.col(2) = v0;
		premultiplied.col(3) = v1;

		premultiplied = premultiplied * coefficients;
		//

		if(true)
		{
			_a = p0 * 2 - p1 * 2 + v0 + v1;
			_b = p0 * -3 + p1 * 3 - v0 * 2 - v1;
			_c = v0;
			_d = p0;
		}
		else
		{
			_d = p0;
			_b = vec_t(0, 0, 0);
			//_a = 
		}
		_3a = _a * 3;
		_2b = _b * 2;
		_6a = _3a * 2;
	}

	inline vec_t at(float_t const t) const
	{
/*		Eigen::Matrix< float_t, 4, 1 > degree_mat;
		degree_mat(3) = (float_t)1;
		degree_mat(2) = t;
		degree_mat(1) = t * t;
		degree_mat(0) = t * degree_mat(1);

		return premultiplied * degree_mat;
*/
		float_t const t2 = t * t;
		return _a * t2 * t + _b * t2 + _c * t + _d;
	}

	inline float_t component_at(int const comp, float_t const t) const
	{
		float_t const t2 = t * t;
		return _a[comp] * t2 * t + _b[comp] * t2 + _c[comp] * t + _d[comp];
	}

	inline vec_t tangent_at(float_t const t) const
	{
		return first_derivative_at(t);
	}

	inline vec_t direction_at(float_t const t) const
	{
		return tangent_at(t).normalized();
	}

	// TODO: Issue when first and second derivatives are parallel, resulting in divide by 0.
	// Not sure what the deal is with this.
	inline vec_t normal_at(float_t const t, vec_t const& normalized_tangent) const
	{
		vec_t const binormal = first_derivative_at(t).cross(second_derivative_at(t)).normalized();
		return binormal.cross(normalized_tangent);
	}

	inline vec_t normal_at(float_t const t) const
	{
		vec_t const first = first_derivative_at(t);
		vec_t const binormal = first.cross(second_derivative_at(t)).normalized();
		return binormal.cross(first.normalized());
	}

	inline float_t length(float_t const t0 = (float_t)0, float_t const t1 = (float_t)1)
	{
		// Approximate by subdividing
		float_t const threshold = 0.1f;	// TODO:
		
		float_t const mid = (t0 + t1) / 2;
		vec_t const p0 = at(t0);
		vec_t const p1 = at(t1);
		float_t const p0p1_dist = (p0 - p1).norm();
		vec_t const p_mid = at(mid);
//		vec_t const geom_mid = (p0 + p1) / 2;
//		float_t const offset = (p_mid - geom_mid).norm();
//		float_t const proportion = offset / p0p1_dist;
		float_t const p0pmidp1_dist = (p_mid - p0).norm() + (p1 - p_mid).norm();
		float_t const proportion = p0pmidp1_dist / p0p1_dist - 1.0f;	// TODO: Not sure about using this test
		if(proportion > threshold)
		{
			return length(t0, mid) + length(mid, t1);
		}
		else
		{
			return p0p1_dist;
		}
	}

#if 0
	inline float_t length(float_t const t0 = (float_t)0, float_t const t1 = (float_t)1)
	{
		// Pretty sure this is wrong!

		// TODO: Precalc more coefficient combinations maybe
		vec_t p0 = at(t0);
		vec_t p1 = at(t1);
		vec_t distances;
		// Solve quadratic equation of the spline's derivative
		vec_t _4b2_12ac = _2b.cwiseProduct(_2b) - _6a.cwiseProduct(_c) * 2;
		for(int i = 0; i < 3; ++i)
		{
			if(_4b2_12ac[i] <= 0)
			{
				// Spline has no minima/maxima in this component, so tangent is always positive or always negative.
				// This means we can integrate along length in one go, which amounts to taking the component difference
				// at points t0 and t1 (since it at no point flips direction)
				distances[i] = p1[i] - p0[i];	// Sign unimportant since will later be squared
			}
			else
			{
				// Spline has a minimum and a maximum in this component, at following values of t
				// Sign of the tangent in this component is different between these two t values than it is outside them.
				float_t t_mm_0 = (-_2b[i] + std::sqrt(_4b2_12ac[i])) / (2 * _3a[i]);
				float_t t_mm_1 = (-_2b[i] - std::sqrt(_4b2_12ac[i])) / (2 * _3a[i]);
				if(t_mm_0 > t_mm_1)
				{
					std::swap(t_mm_0, t_mm_1);
				}
				// Assuming argument t0 < t1
				if(t_mm_0 >= t1 || t_mm_1 <= t0 || (t_mm_0 <= t0 && t_mm_1 >= t1))
				{
					// Min/maxima fall outside requested interval
					distances[i] = p1[i] - p0[i];
				}
				else if(t_mm_0 > t0)
				{
					if(t_mm_1 < t1)
					{
						// So both min/maxima fall within [t0, t1]
						distances[i] = std::abs(p1[i] - p0[i]) + 2 * std::abs(component_at(i, t_mm_0) - component_at(i, t_mm_1));
					}
					else
					{
						// t0 < t_mm_0 < t1
						float_t comp_at_mm = component_at(i, t_mm_0);
						// Note order here is important to ensure both parts are of same sign (whatever it may be)
						// so that we don't need an abs() call
						distances[i] = (comp_at_mm - p0[i]) + (comp_at_mm - p1[i]);
					}
				}
				else
				{
					// Must be case that t0 < t_mm_1 < t1
					float_t comp_at_mm = component_at(i, t_mm_1);
					// Note order here is important to ensure both parts are of same sign (whatever it may be)
					// so that we don't need an abs() call
					distances[i] = (comp_at_mm - p0[i]) + (comp_at_mm - p1[i]);
				}
			}
		}

		return distances.norm();
	}
#endif

	inline mesh< float_t > generate_line_mesh(size_t num_segments, float_t t0 = (float_t)0, float_t t1 = (float_t)1) const
	{
		typedef mesh< float_t > mesh_t;
		typedef mesh_t::vertex_t vertex_t;
		typedef mesh_t::index_t index_t;

		std::vector< vertex_t > vertices(num_segments + 1);
		for(size_t i = 0; i < num_segments + 1; ++i)
		{
			vertices[i] = at(t0 + (t1 - t0) * i / num_segments);
		}

		std::vector< index_t > indices(num_segments * 2);
		for(size_t i = 0; i < num_segments; ++i)
		{
			indices[i * 2] = i;
			indices[i * 2 + 1] = i + 1;
		}

		mesh< float_t > m;
		m.set_vertices(vertices);
		m.set_indices(indices);
		return m;
	}

	template <
		typename ColorFtr,		// provides color_t operator() (float_t t, float_t theta)
		typename TexCoordFtr	// provides tex_coord_t operator() (float_t t, float_t theta)
	>
	inline mesh< float_t > generate_tube_mesh(
		float_t radius,
		size_t num_segments,
		size_t num_faces,
		float_t t0 = (float_t)0,
		float_t t1 = (float_t)1,
		mesh_base::vtx_elements elements = mesh_base::vtx_elements::Positions,
		ColorFtr color_ftr = ColorFtr(),
		TexCoordFtr tex_coord_ftr = TexCoordFtr()
		) const
	{
		typedef mesh< float_t > mesh_t;
		typedef mesh_t::vertex_t vertex_t;
		typedef mesh_t::normal_t normal_t;
		typedef mesh_t::tex_coord_2d_t tex_coord_2d_t;
		typedef mesh_t::color_t color_t;
		typedef mesh_t::index_t index_t;

		size_t const num_vertices = (num_segments + 1) * (num_faces + 1);
		std::vector< vertex_t, Eigen::aligned_allocator< vertex_t > > vertices(num_vertices);
		std::vector< normal_t, Eigen::aligned_allocator< normal_t > > normals(num_vertices);
		std::vector< tex_coord_2d_t, Eigen::aligned_allocator< tex_coord_2d_t > > tex_coords(num_vertices);
		std::vector< color_t, Eigen::aligned_allocator< color_t > > colors(num_vertices);

		size_t idx = 0;
		float_t t = t0;
		vec_t v = direction_at(t);
		//vec_t n = normal_at(t, v);	see note on normal_at method
		vec_t n = any_perpendicular(v).normalized();

		for(size_t i = 0; i < num_segments + 1; ++i)
		{
			assert(!_isnan(v.norm()));
			assert(!_isnan(n.norm()));

			// Center position (on spline)
			vec_t p = at(t);

			assert(!_isnan(p.norm()));

			// Create ring of vertices around p
			for(size_t j = 0; j < num_faces + 1; ++j)
			{
				// Rotate normal around tangent based on j
				vec_t rotated_norm =
					Eigen::AngleAxis< float_t >(2 * boost::math::double_constants::pi * j / num_faces, v) *
					n;
				vertices[idx] = p + rotated_norm * radius;
				if(flags(elements & mesh_base::vtx_elements::Normals))
				{
					normals[idx] = rotated_norm;
				}
				if(flags(elements & mesh_base::vtx_elements::TexCoords))
				{
					tex_coords[idx] = tex_coord_ftr((float_t)i / num_segments, (float_t)j / num_faces);
				}
				if(flags(elements & mesh_base::vtx_elements::Colors))
				{
					colors[idx] = color_ftr((float_t)i / num_segments, (float_t)j / num_faces);
				}
				++idx;
			}

			t = t0 + (t1 - t0) * (i + 1) / num_segments;
			vec_t next_v = direction_at(t);
			//vec_t perp = v.cross(next_v);
			n = Eigen::Quaternion< float_t >::FromTwoVectors(v, next_v) * n;
			v = next_v;
		}

		// Generate indices
		size_t const num_indices = num_segments * num_faces * 6;
		std::vector< index_t > indices(num_indices);
		std::vector< index_t >::iterator it = indices.begin();
		for(size_t i = 0; i < num_segments; ++i)
		{
			size_t num_per_ring = (num_faces + 1);
			size_t base = i * num_per_ring;
			for(size_t j = 0; j < num_faces; ++j)
			{
				*it++ = base + j;
				*it++ = base + j + 1;
				*it++ = base + num_per_ring + j;

				*it++ = base + j + 1;
				*it++ = base + num_per_ring + j + 1;
				*it++ = base + num_per_ring + j;
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
};

// For now, implementing composite spline using a t range of [0, 1], SPLIT EQUALLY BETWEEN N SEGMENTS
// May want to change this in future (see parameterized catmull rom splines, for example)
class cubic_spline
{
public:
	typedef cubic_spline_segment::float_t float_t;
	typedef cubic_spline_segment::vec_t vec_t;

public: // temp public
	size_t num_points;
	std::vector< cubic_spline_segment::vec_t > control_points;
	std::vector< cubic_spline_segment::vec_t > control_tangents;

public:
	cubic_spline()
	{}

private:
	inline std::pair< size_t, float_t > to_local(float_t const t) const
	{
		float_t int_part;
		float_t local_t = std::modf(t * num_segments(), &int_part);
		size_t segment = (size_t)int_part;

		assert(segment < num_segments() || t == (float_t)1.0 && local_t == (float_t)0.0);

		if(segment == num_segments())
		{
			--segment;
			local_t = (float_t)1.0;
		}

		return std::make_pair(segment, local_t);
	}

	inline float_t to_global(size_t const segment, float_t const local_t) const
	{
		return ((float_t)1.0 / num_segments()) * (segment + local_t);
	}

public:
	template < typename PointIterator, typename TangentIterator >
	void set_control_points(size_t num, PointIterator points, TangentIterator tangents)
	{
		assert(num >= 2);

		control_points.resize(num);
		std::copy(points, points + num, control_points.begin());
		control_tangents.resize(num);
		std::copy(tangents, tangents + num, control_tangents.begin());
		num_points = num;
	}

	inline size_t num_segments() const
	{
		return num_points - 1;
	}

	inline vec_t at(float_t const t) const
	{
		size_t segment;
		float_t local_t;
		std::tie(segment, local_t) = to_local(t);

		cubic_spline_segment seg_spline;
		seg_spline.set_constraints(
			control_points[segment],
			control_points[segment + 1],
			control_tangents[segment],
			control_tangents[segment + 1]
			);
		return seg_spline.at(local_t);
	}

	inline vec_t tangent_at(float_t const t) const
	{
		size_t segment;
		float_t local_t;
		std::tie(segment, local_t) = to_local(t);

		cubic_spline_segment seg_spline;
		seg_spline.set_constraints(
			control_points[segment],
			control_points[segment + 1],
			control_tangents[segment],
			control_tangents[segment + 1]
			);
		return seg_spline.tangent_at(local_t);
	}

	inline vec_t direction_at(float_t const t) const
	{
		return tangent_at(t).normalized();
	}

	inline float_t length(float_t const t0 = (float_t)0, float_t const t1 = (float_t)1)
	{
		size_t segment_src;
		float_t local_t_src;
		std::tie(segment_src, local_t_src) = to_local(t0);

		size_t segment_dst;
		float_t local_t_dst;
		std::tie(segment_dst, local_t_dst) = to_local(t1);

		float_t distance = (float_t)0.0;
		for(size_t seg = segment_src; seg <= segment_dst; ++seg)
		{
			cubic_spline_segment seg_spline;
			seg_spline.set_constraints(
				control_points[seg],
				control_points[seg + 1],
				control_tangents[seg],
				control_tangents[seg + 1]
				);

			distance += seg_spline.length(
				seg == segment_src ? local_t_src : (float_t)0.0,
				seg == segment_dst ? local_t_dst : (float_t)1.0
				);
		}

		return distance;
	}

	// TODO: This implementation is making the (incorrect) assumption that distance travelled along a spline segment
	// is linear with t. If this is not okay, options are cardinal splines (catmull rom?), or a divide and conquer sampling
	// where we divide based on the above assumption, then calculate the actual length, and then try again one side or the
	// other until the error is sufficiently small.
	inline float_t t_at_distance(float_t const t_from, float_t const distance) const
	{
		size_t seg;
		float_t t0;
		std::tie(seg, t0) = to_local(t_from);

		float_t rem_distance = distance;
		while(true)
		{
			cubic_spline_segment seg_spline;
			seg_spline.set_constraints(
				control_points[seg],
				control_points[seg + 1],
				control_tangents[seg],
				control_tangents[seg + 1]
				);
			float_t rem_seg_length = seg_spline.length(t0, (float_t)1.0);
			if(rem_distance <= rem_seg_length)
			{
				return to_global(seg, t0 + ((float_t)1.0 - t0) * (rem_distance / rem_seg_length));
			}

			++seg;
			t0 = (float_t)0.0;
			rem_distance -= rem_seg_length;
		}
	}
};



#endif


