
#include "draw_graph_app.h"
#include "text_output.h"

#include "gles_angle/modified/m_esUtil.h"
#include "gles_angle/primitives.h"

#include "util/text_io.h"

#include "graphgen/gen_planar.h"
#include "graphgen/gen_spatial.h"

#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>
#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/is_straight_line_drawing.hpp>
#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/math/special_functions/sign.hpp>
#include <boost/math/constants/constants.hpp>

#include <chrono>
#include <sstream>
#include <string>


draw_graph_app::draw_graph_app(): use_texture_shader(false), graph_connected(false)
{
	rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));

	node_locations = generate_graph_spatial_first();

/*
	scene = new Scene();

	scene->getDefaultCamera()->setOrthoMode(true, 5.0 * SCREEN_WIDTH / SCREEN_HEIGHT, 5.0);
	scene->getDefaultCamera()->setPosition(0.0, 0.0, 1.0);
	scene->getDefaultCamera()->lookAt(poly::Vector3(0, 0, 0), poly::Vector3(0, 1, 0));

	typedef boost::property_map< gk::graph, boost::vertex_index_t >::type index_map_t;
	index_map_t index = boost::get(boost::vertex_index, g);

	gk::g_node_it v_it, v_end;
	for(boost::tie(v_it, v_end) = boost::vertices(g); v_it != v_end; ++v_it)
	{
		gk::g_node u = *v_it;
		poly::Vector3 pos(drawing[u].x, drawing[u].y, 0.0);

		poly::ScenePrimitive* node = new poly::ScenePrimitive(poly::ScenePrimitive::TYPE_SPHERE, 0.05, 10, 10);
		node->setPosition(pos);
		scene->addEntity(node);
	}


	std::cout << "Graph connections:" << std::endl << g;

	core->getInput()->addEventListener(this, poly::InputEvent::EVENT_KEYDOWN);
	core->getInput()->addEventListener(this, poly::InputEvent::EVENT_KEYUP);	
	*/
}

draw_graph_app::~draw_graph_app()
{

}

#if 0
draw_graph_app::drawing_map_t draw_graph_app::generate_graph_topology_first()
{
	// Generate planar g
	gk::ggen::generate_planar_graph(
		0,
		0,
		g,
		rgen
		);

	drawing_map_t drawing;
	levels_drawing(drawing);

	return drawing;
}
#endif

draw_graph_app::node_location_map_t draw_graph_app::generate_graph_spatial_first()
{
	double const len = 4.0;
	gk::ggen::node_coords coords;
	gk::ggen::generate_spatial_nodes(coords, rgen, len, 5.0);
	gk::ggen::generate_unconnected_graph_from_spatial_nodes2(coords, g, g_gen_data, rgen);

	node_location_map_t locations;
	for(size_t i = 0; i < coords.size(); ++i)
	{
		gk::g_node u = boost::vertex(i, g);
		locations[u] = coords[i];
	}

	return locations;
}

#if 0
void draw_graph_app::boost_drawing(drawing_map_t& d)
{
	// Make copy g_mp;
	gk::ud_graph g_mp;
	// TODO: should gk::graph be gk::ud_graph??????????????? Same thing in gen_planar.h
	typedef boost::property_map< gk::graph, boost::vertex_index_t >::type index_map_t;
	typedef boost::iterator_property_map< typename std::vector< gk::udg_node >::iterator,
		index_map_t, gk::udg_node, gk::udg_node& > iso_map_t;

	std::vector< gk::udg_node > iso_vec(boost::num_vertices(g));
	iso_map_t v_map(iso_vec.begin(), boost::get(boost::vertex_index, g));
	boost::copy_graph(g, g_mp, boost::orig_to_copy(v_map));

	// Initialize the interior edge index
	boost::property_map< gk::ud_graph, boost::edge_index_t >::type e_index = boost::get(boost::edge_index, g_mp);
	boost::graph_traits< gk::ud_graph >::edges_size_type edge_count = 0;
	gk::udg_edge_it ei, ei_end;
	for(boost::tie(ei, ei_end) = boost::edges(g_mp); ei != ei_end; ++ei)
	{
		boost::put(e_index, *ei, edge_count++);
	}

	typedef std::vector< std::vector< gk::udg_edge > > embedding_storage_t;
	typedef boost::iterator_property_map <
		embedding_storage_t::iterator,
		boost::property_map< gk::ud_graph, boost::vertex_index_t >::type 
	> embedding_t;

	// Create the planar embedding
	embedding_storage_t embedding_storage(boost::num_vertices(g_mp));
//	embedding_t embedding(embedding_storage.begin(), boost::get(boost::vertex_index, g_mp));
	
	assert(
		boost::boyer_myrvold_planarity_test(
			boost::boyer_myrvold_params::graph = g_mp,
			boost::boyer_myrvold_params::embedding = //&embedding_storage[0]//embedding
				boost::make_iterator_property_map(embedding_storage.begin(), boost::get(boost::vertex_index, g_mp))
		)
	);

	// Now make maximal planar
	boost::make_biconnected_planar(g_mp, boost::make_iterator_property_map(embedding_storage.begin(), boost::get(boost::vertex_index, g_mp)));
		//&embedding_storage[0]);//embedding);

	// Re-initialize the edge index, since we just added a few edges
	edge_count = 0;
	for(boost::tie(ei, ei_end) = boost::edges(g_mp); ei != ei_end; ++ei)
	{
		boost::put(e_index, *ei, edge_count++);
	}

	assert(
		boost::boyer_myrvold_planarity_test(
			boost::boyer_myrvold_params::graph = g_mp,
			boost::boyer_myrvold_params::embedding = boost::make_iterator_property_map(embedding_storage.begin(), boost::get(boost::vertex_index, g_mp))
			//&embedding_storage[0]//embedding
		)
	);

	boost::make_maximal_planar(g_mp, boost::make_iterator_property_map(embedding_storage.begin(), boost::get(boost::vertex_index, g_mp)));
		//&embedding_storage[0]);//embedding);

	// Re-initialize the edge index, since we just added a few edges
	edge_count = 0;
	for(boost::tie(ei, ei_end) = boost::edges(g_mp); ei != ei_end; ++ei)
	{
		boost::put(e_index, *ei, edge_count++);
	}

	assert(
		boost::boyer_myrvold_planarity_test(
			boost::boyer_myrvold_params::graph = g_mp,
			boost::boyer_myrvold_params::embedding = boost::make_iterator_property_map(embedding_storage.begin(), boost::get(boost::vertex_index, g_mp))
			//&embedding_storage[0]//embedding
		)
	);

	// Find a canonical ordering
	std::vector< gk::udg_node > ordering;
	boost::planar_canonical_ordering(g_mp,
		boost::make_iterator_property_map(embedding_storage.begin(), boost::get(boost::vertex_index, g_mp)),
		//&embedding_storage[0]/*embedding*/,
		std::back_inserter(ordering));

	struct coord_t
	{
		std::size_t x;
		std::size_t y;
	};

	// Set up a property map to hold the mapping from vertices to coord_t's
	typedef std::vector< coord_t > straight_line_drawing_storage_t;
	typedef boost::iterator_property_map <
		straight_line_drawing_storage_t::iterator, 
		boost::property_map< gk::ud_graph, boost::vertex_index_t >::type 
	>
	straight_line_drawing_t;

	straight_line_drawing_storage_t straight_line_drawing_storage(boost::num_vertices(g_mp));
	straight_line_drawing_t straight_line_drawing(straight_line_drawing_storage.begin(), boost::get(boost::vertex_index, g_mp));

	// Generate the drawing for g_mp
	boost::chrobak_payne_straight_line_drawing(g_mp, 
		boost::make_iterator_property_map(embedding_storage.begin(), boost::get(boost::vertex_index, g_mp)),
		//&embedding_storage[0]/*embedding*/,
		ordering.begin(), ordering.end(), straight_line_drawing);
	assert(boost::is_straight_line_drawing(g_mp, straight_line_drawing));

	size_t num_nodes = boost::num_vertices(g);
	size_t grid_width = 2 * num_nodes - 4;
	size_t grid_height = num_nodes - 2;
	double cell_width = (double)GRAPH_PLOT_WIDTH / grid_width;
	double cell_height = (double)GRAPH_PLOT_HEIGHT / grid_height;

	gk::g_node_it v_it, v_end;
	for(boost::tie(v_it, v_end) = boost::vertices(g); v_it != v_end; ++v_it)
	{
		gk::g_node u = *v_it;
		coord_t coord(boost::get(straight_line_drawing, v_map[u]));

		d[u] = poly::Vector2(coord.x * cell_width, coord.y * cell_height);
	}
}

void draw_graph_app::levels_drawing(drawing_map_t& d)
{
	// Find start node (TODO: Stored as part of graph type)
	gk::g_node start;
	gk::g_node_it v_it, v_end;
	for(boost::tie(v_it, v_end) = boost::vertices(g); v_it != v_end; ++v_it)
	{
		if(boost::in_degree(*v_it, g) == 0)
		{
			start = *v_it;
			break;
		}
	}

	// Calc shortest paths
	std::vector< int > d_map(boost::num_vertices(g));
	boost::breadth_first_search(g, start,
		boost::visitor(
			boost::make_bfs_visitor(
				boost::record_distances(&d_map[0], boost::on_tree_edge())
				)));

	// Process levels
	size_t max_width = 0;
	gk::ggen::detail::level_map levels;
	for(boost::tie(v_it, v_end) = boost::vertices(g); v_it != v_end; ++v_it)
	{
		size_t sp = d_map[*v_it];
		levels[sp].insert(*v_it);

		max_width = std::max(max_width, levels[sp].size());
	}

	size_t length = levels.size();

	double cell_width = (double)GRAPH_PLOT_WIDTH / length;
	double cell_height = (double)GRAPH_PLOT_HEIGHT / max_width;

	for(size_t lv = 0; lv < length; ++lv)
	{
		size_t offset = 0;
		for(auto it = levels[lv].begin(); it != levels[lv].end(); ++it)
		{
			gk::g_node u = *it;

			d[u] = poly::Vector2(lv * cell_width, offset * cell_height);
			++offset;
		}
	}
}
#endif

void draw_graph_app::increment_graph()
{
	bool added;
	std::tie(added, graph_connected) = gk::ggen::add_graph_edge_spatial_nodes2(g, g_gen_data, rgen);

	if(added)
	{
		std::cout << "Successfully added edge" << std::endl;

		if(graph_connected)
		{
			std::cout << "Graph now connected" << std::endl;
		}
	}
	else
	{
		std::cout << "Unable to add anymore edges" << std::endl;
	}
}

void draw_graph_app::restart_player()
{
	player.active = true;

	player.u = boost::vertex(0, g);
	size_t out = boost::out_degree(player.u, g);
	size_t choice = boost::random::uniform_int_distribution<>(0, out - 1)(rgen);
	gk::g_out_edge_it e_it, e_end;
	boost::tie(e_it, e_end) = boost::out_edges(player.u, g);
	for(size_t i = 0; i < choice; ++i)
	{
		++e_it;
	}
	player.v = boost::target(*e_it, g);

	player.pos = 0.0f;
	player.rot_angle = 0.0f;
	player.rot_speed = 0.0f;
	player.energy_level = 0.0f;
	player.inner_rot_angle = 0.0f;

	cam.set_projection_mode(camera::ProjectionMode::Perspective, 0.01f, 100.0f);
//	cam.set_po
//	cam.look_at(camera::vec_t(0.0, 0.0, -1.0));
}

bool draw_graph_app::setup_shaders()
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

	return true;
}

bool draw_graph_app::initialise(ESContext* context)
{
	if(!setup_shaders())
	{
		return false;
	}

	node_mesh = generate_sphere_mesh(20, 1.0f,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	connection_mesh = generate_cylinder_mesh(20, 1.0f, 1.0f, false,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals | mesh_base::vtx_elements::Colors,
		Eigen::Vector4f(0.0f, 0.7f, 0.0f, 1.0f),
		Eigen::Vector4f(0.7f, 0.0f, 0.0f, 1.0f)
		);
	player_inner_mesh = generate_cylinder_mesh(3, 1.0f, 1.0f, true,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	player_torus_mesh = generate_torus_mesh(20, 20, 1.0f, 1.0f, 0.3f, 0.15f,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);
	player_marker_mesh = generate_conical_mesh(4, 1.0f, 1.0f, false,
		mesh_base::vtx_elements::Positions | mesh_base::vtx_elements::Normals);

	// Setup camera
	float aspect = (GLfloat)context->width / (GLfloat)context->height;

	cam.set_aspect(aspect);
	cam.set_projection_mode(camera::ProjectionMode::Orthographic, -1000.0, 1000.0);
	// Point the camera along -z, so +x is to the right and +y is up
	cam.look_at(camera::vec_t(0.0, 0.0, -1.0));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
/*
	glGenTextures(1, &tex_obj_id);
	assert(tex_obj_id != 0);

	glBindTexture(GL_TEXTURE_2D, tex_obj_id);

	size_t const tex_width = 64;
	size_t const tex_height = 64;
	GLenum tex_color_fmt = GL_RGB;
	std::vector< GLubyte > tex_data;
	tex_data.resize(3 * tex_width * tex_height);	// 1 byte per component, 3 components
	std::vector< GLubyte >::iterator it = tex_data.begin();
	GLubyte red = 0;
	for(size_t y = 0; y < tex_height; ++y)
	{
		GLubyte blue = 0;
		for(size_t x = 0; x < tex_width; ++x)
		{
			*it++ = red;
			*it++ = 0;
			*it++ = blue;
			
			blue += 256 / tex_width;
		}

		if(y < tex_height / 2)
		{
			red += 2 * 256 / tex_height;
		}
		else
		{
			red -= 2 * 256 / tex_height;
		}
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, tex_color_fmt, tex_width, tex_height, 0, tex_color_fmt, GL_UNSIGNED_BYTE, &tex_data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
 
    glBindTexture(GL_TEXTURE_2D, 0);

	GLenum err = glGetError();
*/
	return true;
}

void draw_graph_app::process_keyboard_input(unsigned char c, int x, int y)
{
	double const CameraSpeed = 0.1;

	switch(c)
	{
	case VK_UP:
		if(!player.active)
		{
			cam.move(0.0, CameraSpeed, 0.0);
		}
		break;
	case VK_DOWN:
		if(!player.active)
		{
			cam.move(0.0, -CameraSpeed, 0.0);
		}
		break;
	case VK_LEFT:
		if(!player.active)
		{
			cam.move(-CameraSpeed, 0.0, 0.0);
		}
		break;
	case VK_RIGHT:
		if(!player.active)
		{
			cam.move(CameraSpeed, 0.0, 0.0);
		}
		break;

	case VK_ADD:
		cam.zoom(1.1);
		break;
	case VK_SUBTRACT:
		cam.zoom(1.0 / 1.1);
		break;

	case 'g':
	case 'G':
		if(!player.active)
		{
			increment_graph();
		}
		break;

	case VK_SPACE:
		if(graph_connected)
		{
			restart_player();
		}
		break;

	case VK_ESCAPE:
		{
			::PostQuitMessage(0);
		}
		break;
	}
}

void draw_graph_app::update(ESContext* context, float dtime)
{
	if(player.active)
	{
		float const player_speed = 0.1f + std::abs(player.energy_level);//0.2f;
		float player_move = player_speed * dtime;

		Eigen::Vector3f u_pos(node_locations[player.u][0], node_locations[player.u][1], 0.0f);
		Eigen::Vector3f v_pos(node_locations[player.v][0], node_locations[player.v][1], 0.0f);
		Eigen::Vector3f vec = v_pos - u_pos;
		float conn_length = vec.norm();
		float prop_move = player_move / conn_length;
		player.pos += prop_move;

		if(player.pos >= 1.0)
		{
			// Reached node
			float prop_move_extra = player.pos - 1.0f;
			player_move = prop_move_extra * conn_length;
			player.pos = 0.0f;

			player.u = player.v;
			u_pos = v_pos;
			
			size_t out = boost::out_degree(player.u, g);
			if(out == 0)
			{
				// Reached finish
				player.active = false;

				cam.set_projection_mode(camera::ProjectionMode::Orthographic, -1000.0, 1000.0);
				// Point the camera along -z, so +x is to the right and +y is up
				cam.look_at(camera::vec_t(0.0, 0.0, -1.0));

				return;
			}

			Eigen::Vector3f marker_vec = 
				Eigen::AngleAxis< float >(player.rot_angle, Eigen::Vector3f(0.0f, 0.0f, 1.0f)) *
				Eigen::Vector3f(0.0f, 1.0f, 0.0f);

			gk::g_out_edge_it e_it, e_end;
			gk::g_node best_node;
			float best_dot = -1.0f;
			for(boost::tie(e_it, e_end) = boost::out_edges(player.u, g); e_it != e_end; ++e_it)
			{
				gk::g_edge e = *e_it;
				gk::g_node t = boost::target(e, g);
				Eigen::Vector3f t_pos(node_locations[t][0], node_locations[t][1], 0.0f);
				Eigen::Vector3f dir = (t_pos - u_pos).normalized();
				float dot = dir.dot(marker_vec);
				if(dot >= best_dot)
				{
					best_node = t;
					best_dot = dot;
				}
			}

			player.v = best_node;

			v_pos = Eigen::Vector3f(node_locations[player.v][0], node_locations[player.v][1], 0.0f);
			vec = v_pos - u_pos;
			conn_length = vec.norm();
			prop_move = player_move / conn_length;
			player.pos += prop_move;

			assert(player.pos < 1.0);

			// For now, just increase energy level at every node
			player.energy_level += 0.05f;
		}

		float const player_rot_accel = 10.0f;

		if(::GetAsyncKeyState(VK_LEFT) != 0)
		{
			player.rot_speed += player_rot_accel * dtime;
		}
		else if(::GetAsyncKeyState(VK_RIGHT) != 0)
		{
			player.rot_speed -= player_rot_accel * dtime;
		}
		player.rot_angle += player.rot_speed * dtime;
		player.rot_angle = std::fmod(player.rot_angle, 2 * boost::math::double_constants::pi);

		float const inner_rot_speed = 0.0f + std::abs(player.energy_level) * 5.0f;
		player.inner_rot_angle += inner_rot_speed * dtime;

		Eigen::Vector3f target_pos = u_pos + (v_pos - u_pos) * player.pos;
		Eigen::Vector3f cam_pos(target_pos);
		Eigen::Vector3f dir = vec.normalized();
		cam_pos += dir * -0.3f;
		cam_pos[2] += 0.15f;
		cam.set_position(cam_pos);
		cam.look_at(target_pos, camera::vec_t(0.0f, 0.0f, 1.0f));
	}
}

void draw_graph_app::draw_nodes()
{
	shader_prog& prog = use_texture_shader ? tex_shader : vcolor_shader;
	prog.activate();

	// Leave a bit of the depth range unused so we can draw the player on top of everything else
	glDepthRangef(0.1f, 1.0f);

/*	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_obj_id);
	glUniform1i(tex_unit_loc, 0);
*/
	
	prog.load_vertices(node_mesh.vertex_data());
	prog.load_normals(node_mesh.normal_data());

	// Use constant vertex color
	prog.set_fixed_vertex_color(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	// Load a light shining in direction -z (so direction to the light is +z)
	Eigen::Vector3f vs_to_light = (cam.view_xform() * Eigen::Vector4f(0.0f, 0.0f, 1.0f, 0.0f)).topRows< 3 >();
	prog.load_light_vector(vs_to_light);
	
	for(auto const& nd: node_locations)
	{
		gk::g_node n = nd.first;
		Eigen::Vector2d const& pos = nd.second;

		// Calculate MV and MVP matrices for the node
		auto mv_xform =
			cam.view_xform() *
			Eigen::Translation< float, 3 >(pos[0], pos[1], 0.0) *
			Eigen::Scaling< float >(0.04f, 0.04f, 0.04f);
		auto mvp_xform =
			cam.proj_xform() *
			mv_xform;

		// Load into the shader
		prog.load_mvp_matrix(mvp_xform.matrix());
		prog.load_mv_matrix(mv_xform.matrix());
		prog.load_mv_normals_matrix(mv_xform.matrix().inverse().transpose());

		// Draw the node
		glDrawElements(GL_TRIANGLES, node_mesh.num_indices(), GL_UNSIGNED_SHORT, node_mesh.index_data());
	}

	prog.disable_vertex_array();
	prog.disable_normal_array();

	prog.load_vertices(connection_mesh.vertex_data());
	prog.load_normals(connection_mesh.normal_data());
	prog.load_colors(connection_mesh.color_data());

//	prog.set_fixed_vertex_color(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	gk::g_edge_it e_it, e_end;
	for(boost::tie(e_it, e_end) = boost::edges(g); e_it != e_end; ++e_it)
	{
		gk::g_edge e = *e_it;
		gk::g_node u = boost::source(e, g);
		gk::g_node v = boost::target(e, g);

		Eigen::Vector3f pos_u(node_locations[u][0], node_locations[u][1], 0.0);
		Eigen::Vector3f pos_v(node_locations[v][0], node_locations[v][1], 0.0);
		Eigen::Vector3f vec = pos_v - pos_u;
		double dist = vec.norm();
		vec.normalize();

		// Calculate MV and MVP matrices for the connection
		Eigen::Vector3f pos = pos_u;

		auto mv_xform =
			cam.view_xform() *
			Eigen::Translation< float, 3 >(pos[0], pos[1], 0.0) *
			Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 0.0, 1.0), vec) *
			Eigen::Scaling< float >(0.015f, 0.015f, dist);

		auto mvp_xform =
			cam.proj_xform() *
			mv_xform;

		// Load into the shader
		prog.load_mvp_matrix(mvp_xform.matrix());
		prog.load_mv_matrix(mv_xform.matrix());
		prog.load_mv_normals_matrix(mv_xform.matrix().inverse().transpose());

		// Draw the node
		glDrawElements(GL_TRIANGLES, connection_mesh.num_indices(), GL_UNSIGNED_SHORT, connection_mesh.index_data());
	}

	prog.disable_vertex_array();
	prog.disable_normal_array();
	prog.disable_color_array();

//	glBindTexture(GL_TEXTURE_2D, 0);


	// Player
	if(player.active)
	{
		float const TorusRadius = 0.09f;
		float const InnerRadius = 0.04f;

		// Ensure player is drawn on top
		//glDepthRangef(0.0f, 0.1f);

/*		Eigen::Vector3f u_pos(node_locations[player.u][0], node_locations[player.u][1], 0.0f);
		Eigen::Vector3f v_pos(node_locations[player.v][0], node_locations[player.v][1], 0.0f);
		Eigen::Vector3f vec = v_pos - u_pos;
		Eigen::Vector3f position = u_pos + vec * player.pos;
		vec.normalize();
		auto player_model_xform = 
			Eigen::Translation< float, 3 >(position) *
			Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 1.0, 0.0), vec) *
			// 1. 
			Eigen::AngleAxis< float >(boost::math::double_constants::pi / 2, Eigen::Vector3f(1.0f, 0.0f, 0.0f));
*/
		{
			prog.load_vertices(player_inner_mesh.vertex_data());
			prog.load_normals(player_inner_mesh.normal_data());

			// Use constant vertex color
			prog.set_fixed_vertex_color(Eigen::Vector4f(0.3f, 0.4f, 0.7f, 1.0f));

			Eigen::Vector3f u_pos(node_locations[player.u][0], node_locations[player.u][1], 0.0f);
			Eigen::Vector3f v_pos(node_locations[player.v][0], node_locations[player.v][1], 0.0f);
			Eigen::Vector3f vec = v_pos - u_pos;
			Eigen::Vector3f position = u_pos + vec * player.pos;
			vec.normalize();

			// Calculate MV and MVP matrices for the node
			auto mv_xform =
				cam.view_xform() *
				Eigen::Translation< float, 3 >(position) *
				Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 0.0, 1.0), vec) *
				Eigen::AngleAxis< float >(player.inner_rot_angle, Eigen::Vector3f(0.0, 0.0, 1.0)) *
				Eigen::Scaling< float >(InnerRadius, InnerRadius, InnerRadius);
			auto mvp_xform =
				cam.proj_xform() *
				mv_xform;

			// Load into the shader
			prog.load_mvp_matrix(mvp_xform.matrix());
			prog.load_mv_matrix(mv_xform.matrix());
			prog.load_mv_normals_matrix(mv_xform.matrix().inverse().transpose());

			// Draw the node
			glDrawElements(GL_TRIANGLES, player_inner_mesh.num_indices(), GL_UNSIGNED_SHORT, player_inner_mesh.index_data());

			prog.disable_vertex_array();
			prog.disable_normal_array();
		}

		{
			prog.load_vertices(player_torus_mesh.vertex_data());
			prog.load_normals(player_torus_mesh.normal_data());

			// Use constant vertex color
			prog.set_fixed_vertex_color(Eigen::Vector4f(0.3f, 0.4f, 0.7f, 1.0f));

			Eigen::Vector3f u_pos(node_locations[player.u][0], node_locations[player.u][1], 0.0f);
			Eigen::Vector3f v_pos(node_locations[player.v][0], node_locations[player.v][1], 0.0f);
			Eigen::Vector3f vec = v_pos - u_pos;
			Eigen::Vector3f position = u_pos + vec * player.pos;
			vec.normalize();

			// Calculate MV and MVP matrices for the node
			auto mv_xform =
				cam.view_xform() *
				Eigen::Translation< float, 3 >(position) *
				Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 0.0, 1.0), vec) *
				Eigen::AngleAxis< float >(player.rot_angle, Eigen::Vector3f(0.0, 0.0, 1.0)) *
				Eigen::Scaling< float >(TorusRadius, TorusRadius, TorusRadius);
			auto mvp_xform =
				cam.proj_xform() *
				mv_xform;

			// Load into the shader
			prog.load_mvp_matrix(mvp_xform.matrix());
			prog.load_mv_matrix(mv_xform.matrix());
			prog.load_mv_normals_matrix(mv_xform.matrix().inverse().transpose());

			// Draw the node
			glDrawElements(GL_TRIANGLES, player_torus_mesh.num_indices(), GL_UNSIGNED_SHORT, player_torus_mesh.index_data());

			prog.disable_vertex_array();
			prog.disable_normal_array();
		}

		{
			prog.load_vertices(player_marker_mesh.vertex_data());
			prog.load_normals(player_marker_mesh.normal_data());

			// Use constant vertex color
			prog.set_fixed_vertex_color(Eigen::Vector4f(0.5f, 0.6f, 0.9f, 1.0f));

			Eigen::Vector3f u_pos(node_locations[player.u][0], node_locations[player.u][1], 0.0f);
			Eigen::Vector3f v_pos(node_locations[player.v][0], node_locations[player.v][1], 0.0f);
			Eigen::Vector3f vec = v_pos - u_pos;
			Eigen::Vector3f position = u_pos + vec * player.pos;
			vec.normalize();

			float const MarkerHeight = 0.05f;

			//
			Eigen::Matrix< float, 3, 3 > _m;
			_m.col(2) = vec;
			_m.col(1) = Eigen::Vector3f(0, 0, 1);
			_m.col(0) = Eigen::Vector3f(0, 0, 1).cross(vec);
			Eigen::Quaternionf q_from_m(_m);
			//

			// Calculate MV and MVP matrices for the node
			auto mv_xform =
				cam.view_xform() *
				Eigen::Translation< float, 3 >(position) *
//				Eigen::Quaternion< float >::FromTwoVectors(Eigen::Vector3f(0.0, 1.0, 0.0), vec) *
//				Eigen::AngleAxis< float >(boost::math::double_constants::pi / 2, Eigen::Vector3f(1.0f, 0.0f, 0.0f)) *
				q_from_m *

				Eigen::AngleAxis< float >(player.rot_angle, Eigen::Vector3f(0.0, 0.0, 1.0)) *
				Eigen::AngleAxis< float >(boost::math::double_constants::pi / 2, Eigen::Vector3f(1.0f, 0.0f, 0.0f)) *
				Eigen::Translation< float, 3 >(0.0f, 0.0f, -TorusRadius) *
				Eigen::Scaling< float >(0.5f * MarkerHeight, 0.5f * MarkerHeight, MarkerHeight);
			auto mvp_xform =
				cam.proj_xform() *
				mv_xform;

			// Load into the shader
			prog.load_mvp_matrix(mvp_xform.matrix());
			prog.load_mv_matrix(mv_xform.matrix());
			prog.load_mv_normals_matrix(mv_xform.matrix().inverse().transpose());

			// Draw the node
			glDrawElements(GL_TRIANGLES, player_marker_mesh.num_indices(), GL_UNSIGNED_SHORT, player_marker_mesh.index_data());

			prog.disable_vertex_array();
			prog.disable_normal_array();
		}
	}
}

void draw_graph_app::draw(ESContext* context)
{
	glViewport(0, 0, context->width, context->height);

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_nodes();

	eglSwapBuffers(context->eglDisplay, context->eglSurface);
}

