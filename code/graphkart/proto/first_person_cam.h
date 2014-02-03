// first_person_cam.h

#ifndef __GK_FIRST_PERSON_CAM_H
#define __GK_FIRST_PERSON_CAM_H

#include "ingame_camera.h"
#include "util/orientation.h"


class level_graph;
class agent;

class first_person_cam: public ingame_camera
{
public:

public:
	first_person_cam()
	{}

private:
	virtual vec_t calc_ideal_position(vec_t const& player_pos, orient_t const& player_orient) const;
	virtual orient_t calc_ideal_orientation(vec_t const& player_pos, orient_t const& player_orient) const;
};


#endif

