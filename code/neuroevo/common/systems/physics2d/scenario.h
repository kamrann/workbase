// scenario.h

#ifndef __NE_PHYS2D_SCENARIO_H
#define __NE_PHYS2D_SCENARIO_H

#include "system_sim/system_state_values.h"

#include "b2d_components/core/contacts.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class scenario
		{
		protected:
			virtual bool create(phys2d_system const* sys) = 0;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const = 0;

			virtual void on_contact(b2Contact* contact, b2dc::ContactType ctype) {}
			virtual bool is_complete() const = 0;

			friend class phys2d_system;
		};

	}
}


#endif

