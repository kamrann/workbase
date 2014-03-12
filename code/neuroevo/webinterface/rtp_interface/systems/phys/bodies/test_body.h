// test_body.h

#ifndef __RTP_PHYS_TEST_BODY_H
#define __RTP_PHYS_TEST_BODY_H

#include "../rtp_phys_agent_body_spec.h"
#include "../rtp_phys_agent_body.h"


class b2Body;
class b2RevoluteJoint;

namespace rtp_phys {

	class test_body: public agent_body
	{
	public:
		class spec: public agent_body_spec
		{
		public:
			static spec* create_instance(rtp_param param);

		public:
			virtual agent_body* create_body(b2World* world);

		protected:
			spec();

		protected:
			// parameters? eg. scale, joint strength, speed, etc
		};

	public:
		test_body(spec const& spc, b2World* world);

		virtual b2Vec2 get_position();
		virtual float get_orientation();
		virtual void draw(Wt::WPainter& painter);

	public:
		float m_rear_len;
		float m_fore_len;
		float m_width;

		b2Body* fore;
		b2Body* rear;
		b2RevoluteJoint* joint;
	};

}


#endif


