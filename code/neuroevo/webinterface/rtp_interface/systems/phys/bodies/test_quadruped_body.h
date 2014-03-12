// test_quadruped_body.h

#ifndef __RTP_PHYS_TEST_QUADRUPED_BODY_H
#define __RTP_PHYS_TEST_QUADRUPED_BODY_H

#include "../rtp_phys_agent_body_spec.h"
#include "../rtp_phys_agent_body.h"


class b2Body;
class b2RevoluteJoint;

namespace rtp_phys {

	class test_quadruped_body: public agent_body
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
		test_quadruped_body(spec const& spc, b2World* world);

		virtual b2Vec2 get_position();
		virtual float get_orientation();
		virtual void draw(Wt::WPainter& painter);

	public:
		float m_torso_len;
		float m_torso_width;
		float m_upper_len;
		float m_upper_width;
		float m_lower_len;
		float m_lower_width;

		b2Body* torso;
		b2Body* rear_upper1;
		b2Body* rear_upper2;
		b2Body* rear_lower1;
		b2Body* rear_lower2;
		b2Body* fore_upper1;
		b2Body* fore_upper2;
		b2Body* fore_lower1;
		b2Body* fore_lower2;
		b2RevoluteJoint* rear_hip1;
		b2RevoluteJoint* rear_hip2;
		b2RevoluteJoint* rear_knee1;
		b2RevoluteJoint* rear_knee2;
		b2RevoluteJoint* fore_hip1;
		b2RevoluteJoint* fore_hip2;
		b2RevoluteJoint* fore_knee1;
		b2RevoluteJoint* fore_knee2;
	};

}


#endif


