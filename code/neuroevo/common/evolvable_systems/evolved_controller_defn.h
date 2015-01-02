// evolved_controller_defn.h

#ifndef __WB_EV_SYS_EV_CONTROLLER_DEFN_H
#define __WB_EV_SYS_EV_CONTROLLER_DEFN_H

#include "ev_controller_genetic_mapping.h"

#include "system_sim/controller_defn.h"
#include "system_sim/system_state_values.h"

#include "ddl/ddl.h"
#include "ddl/components/enum_choice.h"


namespace sys {
	namespace ev {

		class evolvable_controller_defn:
			public i_controller_defn
		{
			// i_controller_defn interface
		public:
			virtual std::string get_name() const override;
			virtual ddl::defn_node get_defn(ddl::specifier& spc) override;
			virtual controller_ptr create_controller(ddl::navigator nav) const override;
		};

		class evolvable_controller_impl_defn
		{
		public:
			enum Components {
				CrossoverOp,
				MutationOp,
			};

			typedef ddl::dep_function< i_system_defn const* > sys_defn_fn_t;
			typedef ddl::dep_function< state_value_id_list > state_vals_fn_t;

		public:
			ddl::defn_node get_defn(ddl::specifier& spc);
			std::unique_ptr< i_genetic_mapping > generate(ddl::navigator nav) const;

			evolvable_controller_impl_defn(sys_defn_fn_t& sys_defn_fn, state_vals_fn_t& sv_fn);

		public:
			ddl::enum_choice< std::unique_ptr< i_genetic_mapping > > ev_controller_type_defn;
		};

	}
}



#endif


