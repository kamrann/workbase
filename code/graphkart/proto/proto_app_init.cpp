// proto_app_init.cpp

#include "proto_app.h"

#include "util/graphics3d/primitives.h"
#include "util/text_io.h"
#include "util/cubic_spline.h"


bool proto_app::setup_shaders()
{
	std::string vtx_sh_src = read_file_to_string("C:/Users/Cameron/Documents/GitHub/workbase/resource/graphkart/shaders/tex.vsh");
	std::string frg_sh_src = read_file_to_string("C:/Users/Cameron/Documents/GitHub/workbase/resource/graphkart/shaders/tex.fsh");

	if(!tex_shader.load(vtx_sh_src, frg_sh_src))
	{
		return false;
	}

	vtx_sh_src = read_file_to_string("C:/Users/Cameron/Documents/GitHub/workbase/resource/graphkart/shaders/vcolor.vsh");
	frg_sh_src = read_file_to_string("C:/Users/Cameron/Documents/GitHub/workbase/resource/graphkart/shaders/vcolor.fsh");

	if(!vcolor_shader.load(vtx_sh_src, frg_sh_src))
	{
		return false;
	}

	vtx_sh_src = read_file_to_string("C:/Users/Cameron/Documents/GitHub/workbase/resource/graphkart/shaders/vcolor_unlit.vsh");
	frg_sh_src = read_file_to_string("C:/Users/Cameron/Documents/GitHub/workbase/resource/graphkart/shaders/vcolor_unlit.fsh");

	if(!vcolor_unlit_shader.load(vtx_sh_src, frg_sh_src))
	{
		return false;
	}

	return true;
}

void proto_app::build_node_mesh()
{
	float const NodeRadius = 0.04f;

	node_mesh = generate_sphere_mesh(10, NodeRadius,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	node_mesh.convert_to_wireframe();
}

void proto_app::build_connection_mesh()
{
	float const ConnectionRadius = 0.01f;

	connection_mesh = generate_cylinder_mesh(20, ConnectionRadius, 1.0f, false,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals | mesh_base::vtx_elements::Colors,
		Eigen::Vector4f(0.0f, 0.7f, 0.0f, 1.0f),
		Eigen::Vector4f(0.7f, 0.0f, 0.0f, 1.0f)
		);
}

void proto_app::build_player_meshes()
{
	float const TorusRadius = 0.09f;
	float const InnerRadius = 0.04f;
	float const MarkerHeight = 0.035f;

	player_inner_mesh = generate_cylinder_mesh(3, InnerRadius, InnerRadius, true,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);

	mesh< float > player_torus_mesh = generate_torus_mesh(20, 20, TorusRadius, TorusRadius, 0.25f * TorusRadius, 0.1f * TorusRadius,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	mesh< float > player_marker_mesh = generate_conical_mesh(4, 0.5f * MarkerHeight, MarkerHeight, false,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	player_marker_mesh.transform(
		Eigen::AngleAxis< float >(boost::math::double_constants::pi / 2, Eigen::Vector3f(1.0f, 0.0f, 0.0f)) *
		Eigen::Translation< float, 3 >(0.0f, 0.0f, -TorusRadius)
		);
	player_outer_mesh = player_torus_mesh + player_marker_mesh;
}

void proto_app::build_static_meshes()
{
	build_node_mesh();
	build_connection_mesh();
	build_player_meshes();
}

void proto_app::build_graph_dependent_meshes()
{
	float const SplineRadius = 0.003f;

	size_t const SplineSegments =
#ifdef _DEBUG
		12
#else
		24
#endif
		;
	size_t const SplineFaces =
#ifdef _DEBUG
		8
#else
		16
#endif
		;

	spline_meshes.clear();

	for(auto const& elem: level.splines)
	{
		cubic_spline const& s = elem.second;
		bool is_internal;
		boost::optional< gk::g_edge > e1, e2;
		std::tie(is_internal, e1, e2) = elem.first;

		Eigen::Vector4f col_src, col_dst;
		if(is_internal)
		{
			col_src = col_dst = Eigen::Vector4f(1, 1, 1, 1);
		}
		else
		{
			col_src = Eigen::Vector4f(0, 1, 0, 1.0f);
			col_dst = Eigen::Vector4f(1, 0, 0, 0.1f);
		}

		// TODO: Add necessary methods to cubic_spline
		cubic_spline_segment ss;
		ss.set_constraints(
			s.control_points[0],
			s.control_points[1],
			s.control_tangents[0],
			s.control_tangents[1]
			);
		spline_meshes.push_back(draw_simple_splines ?
			ss.generate_line_mesh(SplineSegments) :
			ss.generate_tube_mesh(SplineRadius, SplineSegments, SplineFaces, 0, 1,
				mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals | mesh_base::vtx_elements::Colors,
				OneDColorFtr(col_src, col_dst),
				TwoDTexCoordFtr())
			);
	}
}

bool proto_app::initialise(ESContext* context)
{
	if(!setup_shaders())
	{
		return false;
	}

	build_static_meshes();
	
	// Setup cameras
	float aspect = (GLfloat)context->width / (GLfloat)context->height;

	free_cam.set_aspect(aspect);
	reset_free_camera();

	switch_to_third_person();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glFrontFace(GL_CW);

	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	return true;
}

