// rtp_phys_agent_body_spec.h

#ifndef __NE_RTP_PHYS_AGENT_BODY_SPEC_H
#define __NE_RTP_PHYS_AGENT_BODY_SPEC_H

#include "../../params/enum_par.h"

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

			Count,
		};

		static std::string const Names[Type::Count];

		class enum_param_type: public rtp_enum_param_type
		{
		public:
			enum_param_type();
		};

		static agent_body_spec* create_instance(rtp_param param);

	protected:
		agent_body_spec(Type t);

	protected:
		Type m_type;

	public:
		virtual agent_body* create_body(b2World* world) = 0;
	};

}


#endif

