// ground_based.cpp

#include "ground_based.h"

//
#include <Wt/WPainter>
//


namespace rtp_phys {

	rtp_named_param_list ground_based_scenario::params()
	{
		rtp_named_param_list p;
		return p;
	}

	ground_based_scenario::ground_based_scenario(rtp_param param)
	{

	}

	boost::any ground_based_scenario::generate_initial_state(rgen_t& rgen) const
	{
		init_state_data data;

		return boost::any(data);
	}

	void ground_based_scenario::load_initial_state(boost::any const& data, phys_system::state& st) const
	{
		st.world->SetGravity(b2Vec2(0.0f, -9.81f));

		{
			b2BodyDef bd;
			b2Body* ground = st.world->CreateBody(&bd);

			b2EdgeShape shape;
			shape.Set(b2Vec2(-GroundSize, 0.0f), b2Vec2(GroundSize, 0.0f));

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.friction = 0.5f;

			ground->CreateFixture(&fd);
		}
	}

	void ground_based_scenario::draw_fixed_objects(Wt::WPainter& painter) const
	{
		painter.drawLine(-GroundSize, 0.0f, GroundSize, 0.0f);
	}
}


