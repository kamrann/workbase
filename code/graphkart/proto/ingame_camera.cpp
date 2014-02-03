// ingame_camera.cpp

#include "ingame_camera.h"
#include "level_graph.h"
#include "agent.h"


void ingame_camera::set_to_ideal(level_graph const& level, agent const& player)
{
	vec_t player_pos = level.get_agent_position(player);
	vec_t player_dir = level.get_agent_direction(player);
	orient_t player_orient = lookat_orientation(player_dir, player.up);

	set_position(calc_ideal_position(player_pos, player_orient));
	set_orientation(calc_ideal_orientation(player_pos, player_orient));
}

