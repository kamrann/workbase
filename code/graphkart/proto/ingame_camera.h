// ingame_camera.h

#ifndef __GK_INGAME_CAMERA_H
#define __GK_INGAME_CAMERA_H

#include "util/camera.h"


class level_graph;
class agent;

class ingame_camera: public camera
{
private:
	virtual vec_t calc_ideal_position(vec_t const& player_pos, orient_t const& player_orient) const = 0;
	virtual orient_t calc_ideal_orientation(vec_t const& player_pos, orient_t const& player_orient) const = 0;
	
public:
	void set_to_ideal(level_graph const& level, agent const& player);
	
	virtual void update(level_graph const& level, agent const& player, float_t dtime)
	{
		set_to_ideal(level, player);
	}
};


#endif

