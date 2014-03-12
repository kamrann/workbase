// test_biped_body.h

#ifndef __RTP_PHYS_TEST_BIPED_BODY_H
#define __RTP_PHYS_TEST_BIPED_BODY_H

#include "../rtp_phys_agent_body_spec.h"
#include "../rtp_phys_agent_body.h"


class b2Body;
class b2RevoluteJoint;

namespace rtp_phys {

	class test_biped_body: public agent_body
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
		test_biped_body(spec const& spc, b2World* world);

		virtual b2Vec2 get_position();
		virtual float get_orientation();
		virtual void draw(Wt::WPainter& painter);

	public:
		float m_torso_len;
		float m_torso_width;
		float m_pelvis_height;
		float m_pelvis_width;
		float m_upper_len;
		float m_upper_width;
		float m_lower_len;
		float m_lower_width;
		float m_foot_radius;

		b2Body* torso;
		b2Body* pelvis;
		b2Body* upper1;
		b2Body* upper2;
		b2Body* lower1;
		b2Body* lower2;
		b2RevoluteJoint* pelvis_joint;
		b2RevoluteJoint* hip1;
		b2RevoluteJoint* hip2;
		b2RevoluteJoint* knee1;
		b2RevoluteJoint* knee2;
	};

}


#endif


