// space_based.cpp

#include "space_based.h"


namespace rtp {

	space_based_scenario::space_based_scenario()
	{

	}

	boost::any space_based_scenario::generate_initial_state(rgen_t& rgen) const
	{
		init_state_data data;

		return boost::any(data);
	}

	void space_based_scenario::load_initial_state(boost::any const& data, b2World* world/*phys_system::state& st*/)
	{
		/*st.*/world->SetGravity(b2Vec2(0.0f, 0.0f));
	}
}


