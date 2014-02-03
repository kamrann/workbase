// first_person_cam.cpp

#include "first_person_cam.h"
#include "level_graph.h"
#include "agent.h"


first_person_cam::vec_t first_person_cam::calc_ideal_position(vec_t const& player_pos, orient_t const& player_orient) const
{
	return player_pos;
}

first_person_cam::orient_t first_person_cam::calc_ideal_orientation(vec_t const& player_pos, orient_t const& player_orient) const
{
	return player_orient;
}


