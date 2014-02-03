// proto_app.cpp

#include "proto_app.h"
#include "third_person_cam.h"
#include "first_person_cam.h"

#include <boost/math/special_functions/sign.hpp>
#include <boost/math/constants/constants.hpp>

#include <chrono>
#include <sstream>
#include <string>


proto_app::proto_app(): texturing_on(false), paused(false), draw_simple_splines(false)
{
	//rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
}

proto_app::~proto_app()
{

}

camera const& proto_app::active_camera() const
{
	if(players_active())
	{
		return *ingame_cam;
	}
	else
	{
		return free_cam;
	}
}

void proto_app::reset_free_camera()
{
	free_cam.set_projection_mode(camera::ProjectionMode::Perspective, 0.01f, 100.0f);
	free_cam.set_position(camera::vec_t(0.0f, -0.5f, -0.5f));
	free_cam.look_at(camera::vec_t(0, 0, 0), camera::vec_t(0, 0, -1));
}

void proto_app::switch_to_third_person()
{
	ingame_cam.swap(boost::shared_ptr< ingame_camera >(new third_person_cam));
	ingame_cam->set_aspect(free_cam.get_aspect());
	ingame_cam->set_projection_mode(camera::ProjectionMode::Perspective, 0.01f, 100.0f);
	if(players_active())
	{
		ingame_cam->set_to_ideal(level, players[0]);
	}
}

void proto_app::switch_to_first_person()
{
	ingame_cam.swap(boost::shared_ptr< ingame_camera >(new first_person_cam));
	ingame_cam->set_aspect(free_cam.get_aspect());
	ingame_cam->set_projection_mode(camera::ProjectionMode::Perspective, 0.01f, 100.0f);
	if(players_active())
	{
		ingame_cam->set_to_ideal(level, players[0]);
	}
}

bool proto_app::players_active() const
{
	return !players.empty();
}

void proto_app::restart_players()
{
	players.clear();

	players.resize(1, agent());
	for(agent& p: players)
	{
		p.init();

		gk::g_node u;
		u = boost::vertex(0, level.g);
		size_t out = boost::out_degree(u, level.g);
		size_t choice = boost::random::uniform_int_distribution<>(0, out - 1)(rgen);
		gk::g_out_edge_it e_it, e_end;
		boost::tie(e_it, e_end) = boost::out_edges(u, level.g);
		for(size_t i = 0; i < choice; ++i)
		{
			++e_it;
		}
		p.at_junction = true;
		p.e1 = boost::none;
		p.n = u;
		p.e2 = *e_it;

		p.pos = 0.0f;

		Eigen::Vector3f forwards = level.get_agent_direction(p);
		Eigen::Vector3f right = Eigen::Vector3f(0, 0, -1).cross(forwards);
		p.up = forwards.cross(right).normalized();

		p.outer_rot_speed = 0.0f;
		p.energy_level = 0.0f;
	}

	ingame_cam->set_to_ideal(level, players[0]);
}

void proto_app::remove_players()
{
	players.clear();
}

void proto_app::new_game()
{
	level = level_graph();
	level.generate_graph(rgen);
	level.generate_junctions(rgen);

#ifdef _DEBUG
	std::vector< std::pair< float_t, gk::g_node > > sorted_nodes;
	gk::g_node_it v_it, v_end;
	for(boost::tie(v_it, v_end) = boost::vertices(level.g); v_it != v_end; ++v_it)
	{
		sorted_nodes.push_back(std::make_pair(level.node_locations[*v_it][1], *v_it));
	}
	std::sort(sorted_nodes.begin(), sorted_nodes.end());

	std::cout << "Nodes by increasing y position:" << std::endl;
	for(auto const& x: sorted_nodes)
	{
		std::cout << x.second << std::endl;
	}

//	debug_props::reg_contents(level.splines);
#endif

	build_graph_dependent_meshes();
}

void proto_app::toggle_pause()
{
	paused = !paused;
}

void proto_app::process_keyboard_input(unsigned char c, int x, int y)
{
	double const CameraSpeed = 0.1;

	switch(c)
	{
	case VK_UP:
		if(!players_active())
		{
			free_cam.move_local(0.0, CameraSpeed, 0.0);
		}
		break;
	case VK_DOWN:
		if(!players_active())
		{
			free_cam.move_local(0.0, -CameraSpeed, 0.0);
		}
		break;
	case VK_LEFT:
		if(!players_active())
		{
			free_cam.move_local(-CameraSpeed, 0.0, 0.0);
		}
		break;
	case VK_RIGHT:
		if(!players_active())
		{
			free_cam.move_local(CameraSpeed, 0.0, 0.0);
		}
		break;
	case VK_PRIOR:
		if(!players_active())
		{
			free_cam.move_local(0.0, 0.0, CameraSpeed);
		}
		break;
	case VK_NEXT:
		if(!players_active())
		{
			free_cam.move_local(0.0, 0.0, -CameraSpeed);
		}
		break;

	case VK_ADD:
		if(!players_active())
		{
			free_cam.zoom(1.1);
		}
		break;
	case VK_SUBTRACT:
		if(!players_active())
		{
			free_cam.zoom(1.0 / 1.1);
		}
		break;

	case 'e':
	case 'E':
		if(!players_active())
		{
			level.increment_graph(rgen);
			build_graph_dependent_meshes();
		}
		break;

	case 'i':
	case 'I':
		{
//			level.incremental_junction_update();
//			build_graph_dependent_meshes();
		}
		break;

	case '1':
		{
			switch_to_third_person();
		}
		break;

	case '2':
		{
			switch_to_first_person();
		}
		break;

	case VK_HOME:
		restart_players();
		break;

	case VK_SPACE:
		new_game();
		break;

	case VK_PAUSE:
		toggle_pause();
		break;

	case VK_ESCAPE:
		{
			::PostQuitMessage(0);
		}
		break;
	}
}

void proto_app::process_mouse_movement(int x, int y)
{
	if(!players_active())
	{
		free_cam.pitch(y / 2000.0f * boost::math::double_constants::pi);
		//free_cam.yaw(x / 1000.0f * boost::math::double_constants::pi);
		free_cam.set_orientation(
			Eigen::AngleAxis< float >(x / 1000.0f * boost::math::double_constants::pi, Eigen::Vector3f(0, 0, -1)) *
			free_cam.get_orientation()
			);
	}
}

void proto_app::update(ESContext* context, float dtime)
{
	// temp
	//dtime *= 0.1f;
	//

	if(paused)
	{
		return;
	}

	if(players_active())
	{
		agent& player = players[0];

		float const base_speed = 0.1f;
		float const player_speed = base_speed + std::abs(player.energy_level);//0.2f;
		float player_move = player_speed * dtime;

		while(player_move > 0.0f)
		{
			bool termination, junction;
			std::tie(termination, junction) = level.move_agent(player, player_move);

			if(termination)
			{
				// Reached finish
				players.clear();
				reset_free_camera();
				return;
			}

			if(junction)
			{
				gk::g_node junction_node = boost::target(*player.e1, level.g);

				// Choose path /////////////
				Eigen::Vector3f marker_vec = 
					player.xform_hier->get_transform(player.outer_xf) * 
					Eigen::Vector3f(0.0f, 1.0f, 0.0f);

				gk::g_out_edge_it e_it, e_end;
				boost::optional< gk::g_node > best_node;
				boost::optional< gk::g_edge > best_edge;
				float best_dot = -1.0f;
				for(boost::tie(e_it, e_end) = boost::out_edges(junction_node, level.g); e_it != e_end; ++e_it)
				{
					gk::g_edge e = *e_it;
					gk::g_node t = boost::target(e, level.g);
					Eigen::Vector3f t_pos = level.node_locations[t].cast< float >();
					Eigen::Vector3f dir = (t_pos - level.node_locations[junction_node].cast< float >()).normalized();
					float dot = dir.dot(marker_vec);
					if(dot >= best_dot)
					{
						best_node = t;
						best_edge = e;
						best_dot = dot;
					}
				}

				player.at_junction = true;
				player.n = junction_node;
				player.e2 = best_edge;
				player.pos = 0;
				////////////////////////////

				// For now, just increase energy level at every node
				player.energy_level += 0.05f;
			}
		}

		float const player_rot_accel = 7.0f;

		if(::GetAsyncKeyState(VK_LEFT) != 0)
		{
			player.outer_rot_speed -= player_rot_accel * dtime;
		}
		else if(::GetAsyncKeyState(VK_RIGHT) != 0)
		{
			player.outer_rot_speed += player_rot_accel * dtime;
		}

//		player.rot_angle += player.rot_speed * dtime;
//		player.rot_angle = std::fmod(player.rot_angle, 2 * boost::math::double_constants::pi);
		(*player.xform_hier)[player.outer_xf] =
			Eigen::AngleAxis< float >(player.outer_rot_speed * dtime, Eigen::Vector3f(0, 0, 1)) *
			(*player.xform_hier)[player.outer_xf];

		float const inner_rot_speed = 0.0f + std::abs(player.energy_level) * 5.0f;
//		player.inner_rot_angle += inner_rot_speed * dtime;
		(*player.xform_hier)[player.inner_xf] =
			Eigen::AngleAxis< float >(inner_rot_speed * dtime, Eigen::Vector3f(0, 0, 1)) *
			(*player.xform_hier)[player.inner_xf];

		ingame_cam->update(level, players[0], dtime);
	}
	else
	{
		if(::GetAsyncKeyState('I') != 0)
		{
			level.incremental_junction_update();
			build_graph_dependent_meshes();
		}
	}
}


