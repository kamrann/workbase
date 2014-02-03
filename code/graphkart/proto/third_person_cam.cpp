// third_person_cam.cpp

#include "third_person_cam.h"
#include "level_graph.h"
#include "agent.h"
#include "util/orientation.h"


third_person_cam::vec_t third_person_cam::calc_ideal_position(vec_t const& player_pos, orient_t const& player_orient) const
{
	vec_t player_dir = player_orient * vec_t(0, 0, 1);
	return vec_t(
		player_pos[0] - player_dir[0] * 0.4f,
		player_pos[1] - player_dir[1] * 0.4f,
		player_pos[2] - player_dir[2] * 0.4f
		);
}

third_person_cam::orient_t third_person_cam::calc_ideal_orientation(vec_t const& player_pos, orient_t const& player_orient) const
{
	vec_t ideal_pos = calc_ideal_position(player_pos, player_orient);

	return lookat_orientation((player_pos - ideal_pos).normalized(), player_orient * vec_t(0, 1, 0));
}

void third_person_cam::update(level_graph const& level, agent const& player, float_t dtime)
{
	vec_t player_pos = level.get_agent_position(player);
	vec_t player_dir = level.get_agent_direction(player);
	orient_t player_orient = lookat_orientation(player_dir, player.up);

	vec_t ideal_pos = calc_ideal_position(player_pos, player_orient);
	orient_t ideal_orientation = calc_ideal_orientation(player_pos, player_orient);

	vec_t cur_pos = get_position();
	// Let's try, moving proportionally to distance, such that in 1 second we would move the whole way (if happened in single step)
	float const cam_move = dtime * move_speed_factor;
	set_position(cur_pos + (ideal_pos - cur_pos) * std::min(cam_move, 1.0f));

	orient_t cur_orientation = get_orientation();
	float const cam_slerp = dtime * slerp_speed_factor;
	orient_t interpolated = cur_orientation.slerp(std::min(cam_slerp, 1.0f), ideal_orientation);
	set_orientation(interpolated);
}


