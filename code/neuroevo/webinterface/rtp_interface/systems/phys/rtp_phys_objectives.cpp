// rtp_phys_objectives.cpp

#include "rtp_phys_objectives.h"

#include "objectives/max_final_x_objective.h"
#include "objectives/max_average_y_objective.h"
#include "objectives/min_final_linear_speed_objective.h"
#include "objectives/maintain_upright_stance_objective.h"
#include "objectives/min_average_ke_objective.h"
#include "objectives/maintain_altitude_objective.h"
#include "objectives/min_nonfoot_contacts_objective.h"


namespace rtp
{
	std::string const agent_objective::Names[] = {
		"Max Final X",
		"Max Average Y",
		"Min Final Linear Speed",
//		"Maintain Upright Stance",
		"Min Average KE",
		"Maintain Altitude",
		"Min Non-Foot Contacts",
	};
}



