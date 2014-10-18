// basic_system.cpp

#include "basic_system.h"
#include "system_drawer.h"


namespace sys {

	bool basic_system::initialize()
	{
		// TODO: This stuff should be one-time initialized, however currently i_system::initialize() is
		// used for system restart, so probably want to separate into 2 step process
		m_sv_bindings.clear();
		m_sv_accessors.clear();
		auto count = initialize_state_value_bindings(m_sv_bindings, m_sv_accessors);
		// TODO: Hold pointers to agents/controllers in basic_system??
		return true;
	}

	state_value_bound_id basic_system::get_state_value_binding(state_value_id svid) const
	{
		return m_sv_bindings.at(svid);
	}

	double basic_system::get_state_value(state_value_bound_id bid) const
	{
		return m_sv_accessors[bid]();
	}

	system_drawer_ptr basic_system::get_drawer() const
	{
		return nullptr;
	}

}



