// basic_agent.h

#ifndef __NE_SYSSIM_BASIC_AGENT_H
#define __NE_SYSSIM_BASIC_AGENT_H

#include "agent.h"


namespace sys {

	class i_system;

	class basic_agent:
		public i_agent
	{
	public:
		virtual void create(i_system* sys) = 0;

	public:
		virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const = 0;
	};

}


#endif

