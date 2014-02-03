// gl_viewer_app.cpp

#include "gl_viewer_app.h"

#include "util/graphics3d/primitives.h"
#include "util/text_io.h"
#include "util/orientation.h"
#include "util/gl_helpers/gl_helpers.h"
#include "util/cubic_spline.h"

#include <boost/math/special_functions/sign.hpp>
#include <boost/math/constants/constants.hpp>

#include <chrono>
#include <sstream>
#include <string>
#include <fstream>
#include <streambuf>


gl_viewer_app::gl_viewer_app()
{

}

gl_viewer_app::~gl_viewer_app()
{

}

bool gl_viewer_app::setup_shaders()
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

bool gl_viewer_app::initialise(ESContext* context)
{
	if(!setup_shaders())
	{
		return false;
	}

	// Create object
	box = generate_cuboid_mesh(1.5f, 0.5f, 1.0f, mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	sphere = generate_sphere_mesh(20, 1.0f, mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	cylinder = generate_cylinder_mesh(20, 1.0f, 5.0f, true, mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	torus = generate_torus_mesh(25, 25, 1.5f, 1.0f, 0.5f, 0.15f, mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	cone = generate_conical_mesh(20, 1.0f, 2.0f, true, mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);

	cubic_spline_segment spl;
	spl.set_constraints(
		Eigen::Vector3f(0, 0, 0),
		Eigen::Vector3f(0, 1, 0),
		Eigen::Vector3f(20, 10, 0),
		Eigen::Vector3f(-20, 10, 0)
		);
	//spline = spl.generate_tube_mesh(0.1f, 24, 16);
	spline = spl.generate_line_mesh(24);

	std::cout << "Spline length: " << spl.length() << std::endl;

	//spline.convert_to_wireframe();

	for(size_t i = 0; i < 7; ++i)
	{
		float t = (float)i / 6;

		Eigen::Vector3f point = spl.at(t);
		spline_points.push_back(point);

		Eigen::Vector3f tangent = spl.tangent_at(t);
		spline_tangents.push_back(tangent * 0.1f);

		Eigen::Vector3f normal = spl.normal_at(t).normalized();
		spline_normals.push_back(normal);

		std::cout << "p = [" << point[0] << ", " << point[1] << ", " << point[2] << "]; " <<
			"v = [" << tangent[0] << ", " << tangent[1] << ", " << tangent[2] << "]; " <<
			"n = [" << normal[0] << ", " << normal[1] << ", " << normal[2] << "]" << std::endl;
	}

	std::cout << "2nd der @ t=0.0: " << spl.second_derivative_at(0) << std::endl;
	std::cout << "2nd der @ t=0.5: " << spl.second_derivative_at(0.5f) << std::endl;
	std::cout << "2nd der @ t=1.0: " << spl.second_derivative_at(1.0f) << std::endl;


	camera_mesh = generate_camera_mesh(0.5f);

	obj_position.setZero();
	obj_orientation = lookat_orientation(Eigen::Vector3f(0, 0, -1));
		//.setIdentity();

	/*/
	obj_position += Eigen::Vector3f(0.5f, 0.5f, 0.5f);
	obj_orientation = base_orientation(
			Eigen::Vector3f(0.0f, 1.0f, 0.0f),
			Eigen::Vector3f(0.0f, 0.0f, 1.0f)
			);
	/*/

	slerp_start.setIdentity();
	slerp_end.setIdentity();
	slerping = false;

	// Setup camera
	float aspect = (GLfloat)context->width / (GLfloat)context->height;

	cam.set_aspect(aspect);
	//cam.set_projection_mode(camera::ProjectionMode::Orthographic, -1000.0, 1000.0);
	cam.set_projection_mode(camera::ProjectionMode::Perspective, 0.001f, 100.0f);
	cam.set_position(0.0, 0.0, -10.0f);
	cam.look_at(camera::vec_t(0.0, 0.0, 0.0));
	cam.zoom(0.5f);

	secondary_cam.set_aspect(aspect);
	secondary_cam.set_projection_mode(camera::ProjectionMode::Perspective, 0.001f, 100.0f);
	secondary_cam.set_position(-5.0f, 0.0, -10.0f);
	secondary_cam.look_at(cam.get_position());
	secondary_cam.zoom(0.5f);

	secondary_cam_view = false;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glFrontFace(GL_CW);	// Since no longer flipping on z, this needs to be changed from default
	glCullFace(GL_BACK);

	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	return true;
}

void gl_viewer_app::rotate_object_world(Eigen::Quaternionf const& q)
{
	obj_orientation = q * obj_orientation;
}

void gl_viewer_app::rotate_object_local(Eigen::Quaternionf const& q)
{
	obj_orientation = obj_orientation * q;
}

camera& gl_viewer_app::active_camera()
{
	return secondary_cam_view ? secondary_cam : cam;
}

void gl_viewer_app::process_keyboard_input(unsigned char c, int x, int y)
{
	double const CameraSpeed = 0.1;
	double const ObjectRotationSpeed = 0.1;

	switch(c)
	{
	case VK_UP:
		if(::GetAsyncKeyState(VK_CONTROL) == 0)
		{
			active_camera().move_local(0.0, CameraSpeed, 0.0);
		}
		else if(!slerping)
		{
			rotate_object_local(Eigen::Quaternionf(
				Eigen::AngleAxis< float >(ObjectRotationSpeed, Eigen::Vector3f(1.0f, 0.0f, 0.0f))));
		}
		break;
	case VK_DOWN:
		if(::GetAsyncKeyState(VK_CONTROL) == 0)
		{
			active_camera().move_local(0.0, -CameraSpeed, 0.0);
		}
		else if(!slerping)
		{
			rotate_object_local(Eigen::Quaternionf(
				Eigen::AngleAxis< float >(-ObjectRotationSpeed, Eigen::Vector3f(1.0f, 0.0f, 0.0f))));
		}
		break;
	case VK_LEFT:
		if(::GetAsyncKeyState(VK_CONTROL) == 0)
		{
			active_camera().move_local(-CameraSpeed, 0.0, 0.0);
		}
		else if(!slerping)
		{
			rotate_object_local(Eigen::Quaternionf(
				Eigen::AngleAxis< float >(-ObjectRotationSpeed, Eigen::Vector3f(0.0f, 1.0f, 0.0f))));
		}
		break;
	case VK_RIGHT:
		if(::GetAsyncKeyState(VK_CONTROL) == 0)
		{
			active_camera().move_local(CameraSpeed, 0.0, 0.0);
		}
		else if(!slerping)
		{
			rotate_object_local(Eigen::Quaternionf(
				Eigen::AngleAxis< float >(ObjectRotationSpeed, Eigen::Vector3f(0.0f, 1.0f, 0.0f))));
		}
		break;
	case VK_PRIOR:
		if(::GetAsyncKeyState(VK_CONTROL) == 0)
		{
			active_camera().move_local(0.0, 0.0, CameraSpeed);
		}
		break;
	case VK_NEXT:
		if(::GetAsyncKeyState(VK_CONTROL) == 0)
		{
			active_camera().move_local(0.0, 0.0, -CameraSpeed);
		}
		break;

	case VK_ADD:
		active_camera().zoom(1.1);
		break;
	case VK_SUBTRACT:
		active_camera().zoom(1.0 / 1.1);
		break;

	case 'c':
	case 'C':
		secondary_cam_view = !secondary_cam_view;
		break;

	case VK_HOME:
		slerp_start = obj_orientation;
		break;
	case VK_END:
		slerp_end = obj_orientation;
		break;

	case VK_SPACE:
		{
			obj_orientation = slerp_start;
			slerp_pos = 0.0f;
			slerping = true;
		}
		break;

	case VK_ESCAPE:
		{
			::PostQuitMessage(0);
		}
		break;
	}
}

void gl_viewer_app::process_mouse_movement(int x, int y)
{
	if(secondary_cam_view && ::GetAsyncKeyState(VK_CONTROL) != 0)
	{
		cam.pitch(y / 2000.0f * boost::math::double_constants::pi);
		//cam.yaw(x / 1000.0f * boost::math::double_constants::pi);
		cam.set_orientation(
			Eigen::AngleAxis< float >(x / 1000.0f * boost::math::double_constants::pi, Eigen::Vector3f(0, 1, 0)) *
			cam.get_orientation()
			);
	}
	else
	{
		active_camera().pitch(y / 2000.0f * boost::math::double_constants::pi);
		//active_camera().yaw(x / 1000.0f * boost::math::double_constants::pi);
		active_camera().set_orientation(
			Eigen::AngleAxis< float >(x / 1000.0f * boost::math::double_constants::pi, Eigen::Vector3f(0, 1, 0)) *
			active_camera().get_orientation()
			);
	}
}

void gl_viewer_app::update(ESContext* context, float dtime)
{
	if(slerping)
	{
		float const SlerpDuration = 4.0f;

		slerp_pos += dtime / SlerpDuration;
		slerp_pos = std::min(slerp_pos, 1.0f);
		obj_orientation = slerp_start.slerp(slerp_pos, slerp_end);

		if(slerp_pos == 1.0f)
		{
			slerping = false;
		}
	}
}
/*
void gl_viewer_app::draw_basis(Eigen::Vector3f const& pos, Eigen::Quaternionf const& orient, Eigen::Vector4f const& color)
{
	float const Size = 1.0f;

	GLfloat const vertices[] = {
		Size, 0.0f, 0.0f,
		0.0f, Size, 0.0f, 
		0.0f, 0.0f, Size,
		0.0f, 0.0f, 0.0f
	};

	GLushort const indices[3 * 2] = {
		0, 3,
		1, 3, 
		2, 3
	};

	static const mesh< float > arrowhead = generate_conical_mesh(10, 0.025f, 0.08f, true,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);

	glDisable(GL_DEPTH_TEST);

	vcolor_unlit_shader.activate();
	vcolor_unlit_shader.load_vertices(vertices);
	vcolor_unlit_shader.set_fixed_vertex_color(color);

	auto mv_xform =
		cam.view_xform() *
		Eigen::Translation< float, 3 >(pos) *
		orient;
	auto mvp_xform =
		cam.proj_xform() *
		mv_xform;
	vcolor_unlit_shader.load_mvp_matrix(mvp_xform.matrix());

	glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, indices);

	vcolor_unlit_shader.disable_vertex_array();

	vcolor_unlit_shader.load_vertices(arrowhead.vertex_data());

	for(int i = 0; i < 3; ++i)
	{
		Eigen::Vector3f axis_vec(0, 0, 0);
		axis_vec[i] = 1.0f;
		Eigen::Vector4f arrow_color(0, 0, 0, 1.0f);
		arrow_color[i] = 1.0f;
		vcolor_unlit_shader.set_fixed_vertex_color(arrow_color);

		auto mv_xform =
			cam.view_xform() *
			Eigen::Translation< float, 3 >(pos) *
			orient *
			Eigen::Quaternionf::FromTwoVectors(Eigen::Vector3f(0.0f, 0.0f, 1.0f), axis_vec) *
			Eigen::Translation< float, 3 >(0.0f, 0.0f, Size);
		auto mvp_xform =
			cam.proj_xform() *
			mv_xform;
		vcolor_unlit_shader.load_mvp_matrix(mvp_xform.matrix());

		glDrawElements(GL_TRIANGLES, arrowhead.num_indices(), GL_UNSIGNED_SHORT, arrowhead.index_data());
	}

	vcolor_unlit_shader.disable_vertex_array();

	glEnable(GL_DEPTH_TEST);
}
*/
void gl_viewer_app::draw_object()
{
	mesh< float > const& active_mesh = spline;
	shader_prog& prog = vcolor_unlit_shader;

	prog.set_fixed_vertex_color(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	Eigen::Transform< float, 3, Eigen::Affine > model_xform = 
		Eigen::Translation< float, 3 >(obj_position) *
		obj_orientation;
	draw_mesh(active_mesh, model_xform, active_camera(), prog,
		//true, Eigen::Vector3f(0, 1, 0), GL_TRIANGLES);
		false, Eigen::Vector3f(0, 0, 0), GL_LINES);

	{
		glDisable(GL_DEPTH_TEST);

		shader_prog& prog = vcolor_unlit_shader;
		prog.activate();
		GLfloat verts[6];
		prog.load_vertices(verts);
		for(size_t i = 0; i < spline_points.size(); ++i)
		{
			float t = (float)i / (spline_tangents.size() - 1);

			verts[0] = spline_points[i][0];
			verts[1] = spline_points[i][1];
			verts[2] = spline_points[i][2];
			verts[3] = spline_points[i][0] + spline_tangents[i][0] * 0.2f;
			verts[4] = spline_points[i][1] + spline_tangents[i][1] * 0.2f;
			verts[5] = spline_points[i][2] + spline_tangents[i][2] * 0.2f;

			prog.set_fixed_vertex_color(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
			glDrawArrays(GL_LINES, 0, 2);

			verts[3] = spline_points[i][0] + spline_normals[i][0] * 0.2f;
			verts[4] = spline_points[i][1] + spline_normals[i][1] * 0.2f;
			verts[5] = spline_points[i][2] + spline_normals[i][2] * 0.2f;

			prog.set_fixed_vertex_color(Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
			glDrawArrays(GL_LINES, 0, 2);
		}

		glEnable(GL_DEPTH_TEST);
	}

/*
	mesh< float > const& active_mesh = cone;
	shader_prog& prog = vcolor_shader;

	prog.set_fixed_vertex_color(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	Eigen::Transform< float, 3, Eigen::Affine > model_xform = 
		Eigen::Translation< float, 3 >(obj_position) *
		obj_orientation;
	draw_mesh(active_mesh, model_xform, active_camera(), prog);
	*/

#if 0
	prog.activate();

/*	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_obj_id);
	glUniform1i(tex_unit_loc, 0);
*/

	prog.load_vertices(active_mesh.vertex_data());
	prog.load_normals(active_mesh.normal_data());

	// Use constant vertex color
	prog.set_fixed_vertex_color(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	// Calculate MV and MVP matrices for the node
	auto mv_xform =
		cam.view_xform() *
		Eigen::Translation< float, 3 >(obj_position) *
		obj_orientation;
//		Eigen::AngleAxis< float >(boost::math::double_constants::pi / 2, Eigen::Vector3f(0.0f, 1.0f, 0.0f));
//		Eigen::Scaling< float >(0.05, 0.05, 0.05);
	auto mvp_xform =
		cam.proj_xform() *
		mv_xform;

	// Load into the shader
	prog.load_mvp_matrix(mvp_xform.matrix());
	//prog.load_mv_matrix(mv_xform.matrix());
	prog.load_mv_normals_matrix(mv_xform.matrix().inverse().transpose());

	// Also load a light shining in direction -y (so direction to the light is y)
	Eigen::Vector3f vs_to_light = (cam.view_xform() * Eigen::Vector4f(0.0f, 1.0f, 0.0f, 0.0f)).topRows< 3 >();
	prog.load_light_vector(vs_to_light);

	// Draw the node
	glDrawElements(GL_TRIANGLES, active_mesh.num_indices(), GL_UNSIGNED_SHORT, active_mesh.index_data());

	prog.disable_vertex_array();
	prog.disable_normal_array();

//	glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

void gl_viewer_app::draw(ESContext* context)
{
	glViewport(0, 0, context->width, context->height);

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_object();

	if(secondary_cam_view)
	{
		draw_camera(
			Eigen::Translation< float, 3 >(cam.get_position()) * cam.get_orientation(),
			active_camera(), vcolor_shader, 0.5f);

		draw_basis(
			Eigen::Translation< float, 3 >(cam.get_position()) * cam.get_orientation(),
			active_camera(),
			vcolor_unlit_shader,
			1.0f,
			true,
			Eigen::Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	}

	draw_basis(
		Eigen::Transform< float, 3, Eigen::Affine >::Identity(),
		active_camera(),
		vcolor_unlit_shader,
		1.5f,
		true,
		Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

	draw_basis(
		Eigen::Translation< float, 3 >(obj_position) * obj_orientation,
		active_camera(),
		vcolor_unlit_shader,
		0.5f,
		true,
		Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

//	draw_basis();
//	draw_basis(obj_position, obj_orientation, Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));

	eglSwapBuffers(context->eglDisplay, context->eglSurface);
}


