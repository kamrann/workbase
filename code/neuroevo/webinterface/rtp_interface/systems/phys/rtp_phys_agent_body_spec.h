// rtp_phys_agent_body_spec.h

#ifndef __NE_RTP_PHYS_AGENT_BODY_SPEC_H
#define __NE_RTP_PHYS_AGENT_BODY_SPEC_H

#include "../../rtp_defs.h"
#include "../../params/enum_par.h"

#include "util/fixed_or_random.h"

#include <boost/random/uniform_real_distribution.hpp>

#include <string>


class b2World;

namespace rtp_phys {

	class agent_body;

	class agent_body_spec
	{
	public:
		enum Type {
			TestCreature,
			TestQuadruped,
			TestBiped,
			Spaceship,

			Count,
			None = Count,
		};

		static std::string const Names[Type::Count];

		class enum_param_type: public rtp_enum_param_type
		{
		public:
			enum_param_type();
		};

		static rtp_named_param_list base_params();
		static void create_base_instance(rtp_param param, agent_body_spec* s);

		static rtp_param_type* params(Type type);
		static agent_body_spec* create_instance(Type type, rtp_param param);

	protected:
		agent_body_spec(Type t);

	public:
		Type m_type;

		// TODO: No longer really sure what exactly the spec thing is for, but if for some kind of instancing, then
		// probably initial position/velocity params shouldn't be part of it, since can be different per instance??
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_pos_x;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_pos_y;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_orientation;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_linear_dir;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_linear_speed;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_ang_vel;

	public:
		virtual agent_body* create_body(b2World* world) = 0;
	};

}


#endif

