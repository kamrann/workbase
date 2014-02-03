// gl_helpers.h

#ifndef __WB_GL_HELPERS_H
#define __WB_GL_HELPERS_H

#include "util/graphics3d/basis_mesh.h"
#include "util/graphics3d/camera_mesh.h"
#include "util/camera.h"

#include "gles_angle/shader_program.h"

#include <GLES2/gl2.h>


inline void draw_mesh(
	mesh< float > const& m,
	Eigen::Transform< float, 3, Eigen::Affine > const& model_xform,
	camera const& cam,
	shader_prog& prog,
	bool lighting = true,
	Eigen::Vector3f to_light = Eigen::Vector3f(0, 1, 0),
	GLenum mode = GL_TRIANGLES
	)
{
	// TODO: Texturing

	prog.activate();
	prog.load_vertices(m.vertex_data());
	if(m.has_normals())
	{
		prog.load_normals(m.normal_data());
	}
	if(m.has_colors())
	{
		prog.load_colors(m.color_data());
	}

	// Calculate MV and MVP matrices for the node
	auto mv_xform =
		cam.view_xform() *
		model_xform;
	auto mvp_xform =
		cam.proj_xform() *
		mv_xform;

	// Load into the shader
	prog.load_mvp_matrix(mvp_xform);
	if(lighting)
	{
		prog.load_mv_normals_matrix(Eigen::Affine3f(mv_xform.linear().inverse().transpose()));

		Eigen::Vector3f vs_to_light = cam.view_xform().linear() * to_light;
		prog.load_light_vector(vs_to_light);
	}

	// Draw the node
	glDrawElements(mode, m.num_indices(), GL_UNSIGNED_SHORT, m.index_data());

	prog.disable_vertex_array();
	prog.disable_normal_array();
	prog.disable_color_array();
}

inline void draw_basis(
	Eigen::Transform< float, 3, Eigen::Affine > const& xform,
	camera const& cam,
	shader_prog& prog,
	float size,
	bool arrows = true,
	Eigen::Vector4f color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)
	)
{
	GLboolean depth_enabled;
	glGetBooleanv(GL_DEPTH_TEST, &depth_enabled);
	glDisable(GL_DEPTH_TEST);

	mesh< float > m = generate_basis_mesh(size, color);
	draw_mesh(m, xform, cam, prog, false, Eigen::Vector3f::Zero(), GL_LINES);

	if(arrows)
	{
		m = generate_basis_arrows_mesh(size);
		draw_mesh(m, xform, cam, prog, false, Eigen::Vector3f::Zero(), GL_TRIANGLES);
	}

	if(depth_enabled != 0)
	{
		glEnable(GL_DEPTH_TEST);
	}
}

inline void draw_camera(
	Eigen::Transform< float, 3, Eigen::Affine > const& xform,
	camera const& active_cam,
	shader_prog& prog,
	float size
	)
{
	static mesh< float > cam_mesh = generate_camera_mesh(1.0f);

	Eigen::Affine3f scaling(Eigen::Scaling(size));
	draw_mesh(cam_mesh, xform * scaling, active_cam, prog);
}


#endif

