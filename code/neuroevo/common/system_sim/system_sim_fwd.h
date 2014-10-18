// system_sim_fwd.h

#ifndef __NE_SYSTEM_SIM_FWD_H
#define __NE_SYSTEM_SIM_FWD_H

#include <memory>


namespace sys
{
	class i_system_defn;
	class i_system;
	class i_agent_defn;
	class i_agent;
	class i_controller_defn;
	class i_controller;
	class i_observer;

	class i_system_drawer;

	class interactive_input;

	typedef std::unique_ptr< i_system_defn > system_defn_ptr;
	typedef std::unique_ptr< i_agent_defn > agent_defn_ptr;
	typedef std::unique_ptr< i_controller_defn > controller_defn_ptr;

	typedef std::unique_ptr< i_system > system_ptr;
	typedef std::unique_ptr< i_agent > agent_ptr;
	typedef std::unique_ptr< i_controller > controller_ptr;

	typedef std::unique_ptr< i_system_drawer > system_drawer_ptr;
}


#endif

