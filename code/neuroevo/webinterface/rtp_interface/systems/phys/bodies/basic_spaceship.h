// basic_spaceship.h

#ifndef __RTP_PHYS_BASIC_SPACESHIP_H
#define __RTP_PHYS_BASIC_SPACESHIP_H

#include "../rtp_phys_agent_body_spec.h"
#include "simple_rigid_body.h"

#include "thrusters/thruster.h"


class b2Body;

namespace rtp_phys {

	class basic_spaceship: public simple_rigid_body
	{
	public:
		class spec: public agent_body_spec
		{
		public:
			static rtp_param_type* params();
			static spec* create_instance(rtp_param param);

			static YAML::Node get_schema(YAML::Node const& param_vals);

		public:
			virtual agent_body* create_body(b2World* world);

		protected:
			spec();

		protected:
			// parameters? eg. scale, joint strength, speed, etc
		};

	public:
		basic_spaceship(spec const& spc, b2World* world);

		virtual void draw(Wt::WPainter& painter) const;

	public:
		boost::shared_ptr< thruster_config< WorldDimensionality::dim2D > > m_t_cfg;
		thruster_system< WorldDimensionality::dim2D > m_t_system;

		float m_half_width;
	};

}


#endif


