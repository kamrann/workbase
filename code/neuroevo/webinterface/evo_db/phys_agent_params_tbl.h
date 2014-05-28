// phys_agent_params_tbl.h

#ifndef __GADB_PHYS_AGENT_PARAMS_H
#define __GADB_PHYS_AGENT_PARAMS_H

#include "table_traits.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class phys_sys_params;
class phys_agent_type;

class phys_agent_params
{
public:
	dbo::ptr< phys_sys_params >	system;
	float init_pos_x;
	float init_pos_y;
	float init_orient;
	float init_linear_motion_angle;
	float init_linear_spd;
	float init_angular_vel;
	dbo::ptr< phys_agent_type > type;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, system, "system", dbo::NotNull | dbo::OnDeleteCascade);
		dbo::field(a, init_pos_x, "initial position (x)");
		dbo::field(a, init_pos_y, "initial position (y)");
		dbo::field(a, init_orient, "initial orientation");
		dbo::field(a, init_linear_motion_angle, "initial linear motion angle");
		dbo::field(a, init_linear_spd, "initial linear speed");
		dbo::field(a, init_angular_vel, "initial angular velocity");

		dbo::belongsTo(a, type, "phys_agent_type", dbo::NotNull | dbo::OnDeleteCascade);
	}
};


#endif


