// basic_system.h

#ifndef __NE_SYSSIM_BASIC_SYSTEM_H
#define __NE_SYSSIM_BASIC_SYSTEM_H

#include "system.h"


namespace sys {

	class basic_system:
		public i_system
	{
	public:
		virtual bool initialize() override;

		virtual state_value_bound_id get_state_value_binding(state_value_id svid) const override;
		virtual double get_state_value(state_value_bound_id bid) const override;

		virtual system_drawer_ptr get_drawer() const override;

	protected:
		virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) = 0;

	protected:
		sv_bindings_t m_sv_bindings;
		sv_accessors_t m_sv_accessors;
	};

}


#endif

