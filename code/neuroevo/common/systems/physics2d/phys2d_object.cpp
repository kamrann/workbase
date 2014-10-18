// phys2d_object.cpp

#include "phys2d_object.h"


namespace sys {
	namespace phys2d {

		state_value_id_list object::get_state_values()
		{
			state_value_id_list svs;
			for(auto sv : s_sv_names.left)
			{
				svs.push_back(sv.second);
			}
			return svs;
		}

		bimap< object::StateValue, std::string > const object::s_sv_names = {
				{ StateValue::PositionX, "pos_x" },
				{ StateValue::PositionY, "pos_y" },
				{ StateValue::LinVelX, "vel_x" },
				{ StateValue::LinVelY, "vel_y" },
				{ StateValue::KE, "ke" },
		};

	}
}



