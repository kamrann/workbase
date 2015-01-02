// controller_defn.h

#ifndef __NE_CONTROLLER_DEFN_H
#define __NE_CONTROLLER_DEFN_H

#include "system_sim_fwd.h"

#include "ddl/ddl.h"

#include <string>


namespace sys {

	class i_controller_defn
	{
	public:
		virtual std::string get_name() const = 0;
		virtual ddl::defn_node get_defn(ddl::specifier& spc) = 0;

		virtual controller_ptr create_controller(ddl::navigator nav) const = 0;

	public:
		virtual ~i_controller_defn() noexcept
		{}
	};

}


#endif

