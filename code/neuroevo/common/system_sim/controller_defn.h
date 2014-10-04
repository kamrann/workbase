// controller_defn.h

#ifndef __NE_CONTROLLER_DEFN_H
#define __NE_CONTROLLER_DEFN_H

#include "system_sim_fwd.h"

#include "params/param_fwd.h"

#include <string>


namespace sys {

	class i_controller_defn
	{
	public:
		virtual std::string get_name() const = 0;
		virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const = 0;

		virtual controller_ptr create_controller() const = 0;

	public:
		virtual ~i_controller_defn() noexcept
		{}
	};

}


#endif

