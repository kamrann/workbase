// evolved_system_domain_defn.h

#ifndef __WB_EV_SYS_DOMAIN_DEFN_H
#define __WB_EV_SYS_DOMAIN_DEFN_H

#include "evolved_system_domain.h"
#include "evolved_controller_defn.h"
#include "objective_defn.h"

//#include "params/param_fwd.h"
//#include "params/dynamic_enum_schema.h"
#include "ddl/ddl.h"
#include "ddl/components/enum_choice.h"


namespace sys {
	namespace ev {

		class evolved_system_domain_defn
		{
		public:
			ddl::defn_node get_defn(ddl::specifier& spc);
			std::unique_ptr< evolved_system_domain > generate(ddl::navigator nav);

			evolved_system_domain_defn();
			void register_system_type(system_defn_ptr defn);

		protected:

		public:
			ddl::enum_choice< system_ptr > system_defn;
			evolvable_controller_impl_defn ev_con_defn;
			objective_defn obj_defn;

			std::map< std::string, std::shared_ptr< i_system_defn > > sys_defn_mp;

			ddl::dep_function< i_system_defn const* > current_system_defn_fn_;
			ddl::dep_function< state_value_id_list > state_vals_fn_;
		};

	}
}



#endif


