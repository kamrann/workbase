// space_based.cpp

#include "space_based.h"


namespace rtp_phys {

	rtp_named_param_list space_based_scenario::params()
	{
		rtp_named_param_list p;
		return p;
	}

	space_based_scenario::space_based_scenario(rtp_param param)
	{

	}

	boost::any space_based_scenario::generate_initial_state(rgen_t& rgen) const
	{
		init_state_data data;

		return boost::any(data);
	}

	void space_based_scenario::load_initial_state(boost::any const& data, phys_system::state& st) const
	{
		st.world->SetGravity(b2Vec2(0.0f, 0.0f));
	}
}


