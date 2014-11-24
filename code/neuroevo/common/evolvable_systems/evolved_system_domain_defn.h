// evolved_system_domain_defn.h

#ifndef __WB_EV_SYS_DOMAIN_DEFN_H
#define __WB_EV_SYS_DOMAIN_DEFN_H

#include "evolved_system_domain.h"
#include "evolved_controller_defn.h"
#include "objective.h"

#include "params/param_fwd.h"
#include "params/dynamic_enum_schema.h"


namespace sys {
	namespace ev {

		class evolved_system_domain_defn
		{
		public:
			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, int component)
			{}
			std::unique_ptr< evolved_system_domain > generate(prm::param_accessor acc);

			evolved_system_domain_defn();
			void register_system_type(system_defn_ptr defn);

		protected:
			i_system_defn const* current_system_defn(prm::param_accessor acc);

		public:
			prm::dynamic_enum_schema< system_ptr > system_defn;
			evolvable_controller_impl_defn ev_con_defn;
			objective_defn obj_defn;

			std::map< std::string, std::shared_ptr< i_system_defn > > sys_defn_mp;
		};

	}
}



#endif


