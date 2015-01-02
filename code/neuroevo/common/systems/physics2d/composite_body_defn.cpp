// composite_body_defn.cpp

#include "composite_body_defn.h"
#include "phys2d_revolute_joint.h"


namespace sys {
	namespace phys2d {

		state_value_id_list composite_body_defn::get_state_values() const
		{
			state_value_id_list svs;
			auto rev_svs = revolute_joint::get_state_values();
			for(auto const& rev : m_revolutes)
			{
/*				svs.push_back(state_value_id::from_string(rev) + "angle");
				svs.push_back(state_value_id::from_string(rev) + "speed");
				svs.push_back(state_value_id::from_string(rev) + "applied");
				svs.push_back(state_value_id::from_string(rev) + "reaction_Fx");
				svs.push_back(state_value_id::from_string(rev) + "reaction_Fy");
				svs.push_back(state_value_id::from_string(rev) + "reaction_T");
*/
				for(auto const& rev_svid : rev_svs)
				{
					svs.push_back(state_value_id::from_string(rev) + rev_svid);
				}
			}

			return svs;
		}
		
		void composite_body_defn::add_revolute(std::string name)
		{
			m_revolutes.push_back(name);
		}

		void composite_body_defn::clear_revolutes()
		{
			m_revolutes.clear();
		}

	}
}



