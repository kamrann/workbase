// proto_app_drawing.cpp

#include "proto_app.h"
#include "util/gl_helpers/gl_helpers.h"


void proto_app::draw_connections()
{
	gk::graph const& g = level.g;

	shader_prog& prog = texturing_on ? tex_shader : (draw_simple_splines ? vcolor_unlit_shader : vcolor_shader);
	
	// Use constant vertex color
	prog.set_fixed_vertex_color(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
/*
	gk::g_edge_it e_it, e_end;
	for(boost::tie(e_it, e_end) = boost::edges(g); e_it != e_end; ++e_it)
	{
		gk::g_edge e = *e_it;
		gk::g_node u = boost::source(e, g);
		gk::g_node v = boost::target(e, g);

		Eigen::Vector3f pos_u = level.node_junctions[u].out_edge_interfaces[e].first;//level.node_locations[u];
		Eigen::Vector3f pos_v = level.node_junctions[v].in_edge_interfaces[e].first;//level.node_locations[v];
		Eigen::Vector3f vec = pos_v - pos_u;
		double dist = vec.norm();
		vec.normalize();

		// Calculate MV and MVP matrices for the connection
		Eigen::Vector3f pos = pos_u.cast< float >();

		Eigen::Affine3f model_xform =
			Eigen::Translation< float, 3 >(pos) *
			Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 0.0, 1.0), vec.cast< float >()) *
			Eigen::Scaling< float >(1.0f, 1.0f, dist);
		draw_mesh(connection_mesh, model_xform, cam, prog, true, Eigen::Vector3f(0, 0, -1));
	}
*/
	/////////////////////////////

	for(mesh< float > const& m: spline_meshes)
	{
		draw_mesh(m, Eigen::Affine3f::Identity(), active_camera(), prog,
			!draw_simple_splines, Eigen::Vector3f(0, 0, -1),
			draw_simple_splines ? GL_LINES : GL_TRIANGLES);
	}

	/////////////////////////////

/*	prog.activate();

	prog.load_vertices(connection_mesh.vertex_data());
	prog.load_normals(connection_mesh.normal_data());
	prog.load_colors(connection_mesh.color_data());

	gk::g_edge_it e_it, e_end;
	for(boost::tie(e_it, e_end) = boost::edges(g); e_it != e_end; ++e_it)
	{
		gk::g_edge e = *e_it;
		gk::g_node u = boost::source(e, g);
		gk::g_node v = boost::target(e, g);

		Eigen::Vector3d pos_u = level.node_locations[u];
		Eigen::Vector3d pos_v = level.node_locations[v];
		Eigen::Vector3d vec = pos_v - pos_u;
		double dist = vec.norm();
		vec.normalize();

		// Calculate MV and MVP matrices for the connection
		Eigen::Vector3f pos = pos_u.cast< float >();

		auto mv_xform =
			cam.view_xform() *
			Eigen::Translation< float, 3 >(pos) *
			Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 0.0, 1.0), vec.cast< float >()) *
			Eigen::Scaling< float >(1.0f, 1.0f, dist);

		auto mvp_xform =
			cam.proj_xform() *
			mv_xform;

		// Load into the shader
		prog.load_mvp_matrix(mvp_xform);
		prog.load_mv_matrix(mv_xform);
		prog.load_mv_normals_matrix(mv_xform.matrix().inverse().transpose());

		// Draw the node
		glDrawElements(GL_TRIANGLES, connection_mesh.num_indices(), GL_UNSIGNED_SHORT, connection_mesh.index_data());
	}

	prog.disable_vertex_array();
	prog.disable_normal_array();
	prog.disable_color_array();
	*/
}

void proto_app::draw_nodes()
{
	shader_prog& prog = vcolor_unlit_shader;
	prog.activate();

/*	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_obj_id);
	glUniform1i(tex_unit_loc, 0);
*/
	
	prog.load_vertices(node_mesh.vertex_data());
	//prog.load_normals(node_mesh.normal_data());

	// Load a light shining in direction -z (so direction to the light is +z)
//	Eigen::Vector3f vs_to_light = cam.view_xform().linear() * Eigen::Vector3f(0.0f, 0.0f, -1.0f));
//	prog.load_light_vector(vs_to_light);
	
	for(auto const& nd: level.node_locations)
	{
		gk::g_node n = nd.first;
		Eigen::Vector3f pos = nd.second.cast< float >();

		// Calculate MV and MVP matrices for the node
		auto mv_xform =
			active_camera().view_xform() *
			Eigen::Translation< float, 3 >(pos);
		auto mvp_xform =
			active_camera().proj_xform() *
			mv_xform;

		// Load into the shader
		prog.load_mvp_matrix(mvp_xform);
		//prog.load_mv_matrix(mv_xform);
		//prog.load_mv_normals_matrix(Eigen::Affine3f(mv_xform.linear().inverse().transpose()));

		// Use constant vertex color
		Eigen::Vector4f color = boost::out_degree(n, level.g) > 0 ?
			Eigen::Vector4f(1.0f, 1.0f, 0.0f, 0.5f) :
			Eigen::Vector4f(1.0f, 0.0f, 0.0f, 0.5f);
		prog.set_fixed_vertex_color(color);

		// Draw the node
		glDrawElements(GL_LINES, node_mesh.num_indices(), GL_UNSIGNED_SHORT, node_mesh.index_data());
	}

	prog.disable_vertex_array();
	//prog.disable_normal_array();

//	glBindTexture(GL_TEXTURE_2D, 0);
}

void proto_app::draw_players()
{
	shader_prog& prog = texturing_on ? tex_shader : vcolor_shader;
	prog.activate();

	// Load a light shining in direction -z (so direction to the light is +z)
	Eigen::Vector3f vs_to_light = active_camera().view_xform().linear() * Eigen::Vector3f(0.0f, 0.0f, -1.0f);
	prog.load_light_vector(vs_to_light);

	for(agent const& p: players)
	{
//		float const TorusRadius = 0.09f;
//		float const InnerRadius = 0.04f;

/*		gk::g_node u = boost::source(p.e, level.g);
		gk::g_node v = boost::target(p.e, level.g);
		Eigen::Vector3d u_pos = level.node_locations[u];
		Eigen::Vector3d v_pos = level.node_locations[v];
		Eigen::Vector3d vec = v_pos - u_pos;
		Eigen::Vector3d position = u_pos + vec * p.pos;
		vec.normalize();
*/
		Eigen::Vector3f position = level.get_agent_position(p);
		Eigen::Vector3f vec = level.get_agent_direction(p);
		
		{
			prog.load_vertices(player_inner_mesh.vertex_data());
			prog.load_normals(player_inner_mesh.normal_data());

			// Use constant vertex color
			prog.set_fixed_vertex_color(Eigen::Vector4f(0.3f, 0.4f, 0.7f, 1.0f));

			// Calculate MV and MVP matrices for the node
			auto mv_xform =
				active_camera().view_xform() *
				Eigen::Translation< float, 3 >(position.cast< float >()) *
				//Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 1.0, 0.0), vec.cast< float >()) *
				lookat_orientation(vec.cast< float >(), p.up) *
				p.xform_hier->get_transform(p.inner_xf);
			auto mvp_xform =
				active_camera().proj_xform() *
				mv_xform;

			// Load into the shader
			prog.load_mvp_matrix(mvp_xform);
			prog.load_mv_matrix(mv_xform);
			prog.load_mv_normals_matrix(Eigen::Affine3f(mv_xform.linear().inverse().transpose()));

			// Draw the node
			glDrawElements(GL_TRIANGLES, player_inner_mesh.num_indices(), GL_UNSIGNED_SHORT, player_inner_mesh.index_data());

			prog.disable_vertex_array();
			prog.disable_normal_array();
		}

		{
			prog.load_vertices(player_outer_mesh.vertex_data());
			prog.load_normals(player_outer_mesh.normal_data());

			// Use constant vertex color
			prog.set_fixed_vertex_color(Eigen::Vector4f(0.3f, 0.4f, 0.7f, 1.0f));

			// Calculate MV and MVP matrices for the node
			auto mv_xform =
				active_camera().view_xform() *
				Eigen::Translation< float, 3 >(position.cast< float >()) *
				//Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 1.0, 0.0), vec.cast< float >()) *
				lookat_orientation(vec.cast< float >(), p.up) *
				p.xform_hier->get_transform(p.outer_xf);
			auto mvp_xform =
				active_camera().proj_xform() *
				mv_xform;

			// Load into the shader
			prog.load_mvp_matrix(mvp_xform);
			prog.load_mv_matrix(mv_xform);
			prog.load_mv_normals_matrix(Eigen::Affine3f(mv_xform.linear().inverse().transpose()));

			// Draw the node
			glDrawElements(GL_TRIANGLES, player_outer_mesh.num_indices(), GL_UNSIGNED_SHORT, player_outer_mesh.index_data());

			prog.disable_vertex_array();
			prog.disable_normal_array();
		}
	}
}

void proto_app::draw(ESContext* context)
{
	glViewport(0, 0, context->width, context->height);

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_connections();
	draw_nodes();
	draw_players();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	draw_connections();
//	draw_nodes();
	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
	draw_basis(Eigen::Affine3f::Identity(), active_camera(), vcolor_unlit_shader, 0.25f);
	glEnable(GL_DEPTH_TEST);

	eglSwapBuffers(context->eglDisplay, context->eglSurface);
}

