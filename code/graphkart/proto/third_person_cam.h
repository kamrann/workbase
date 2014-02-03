// third_person_cam.h

#ifndef __GK_THIRD_PERSON_CAM_H
#define __GK_THIRD_PERSON_CAM_H

#include "ingame_camera.h"
#include "util/orientation.h"


class level_graph;
class agent;

class third_person_cam: public ingame_camera
{
public:
	float_t move_speed_factor;
	float_t slerp_speed_factor;

public:
	third_person_cam(): move_speed_factor(1.0f), slerp_speed_factor(1.0f)
	{}

private:
	virtual vec_t calc_ideal_position(vec_t const& player_pos, orient_t const& player_orient) const;
	virtual orient_t calc_ideal_orientation(vec_t const& player_pos, orient_t const& player_orient) const;

public:
	virtual void update(level_graph const& level, agent const& player, float_t dtime);
};


#endif

