// evolved_controller_defn.h

#ifndef __WB_EV_SYS_EV_CONTROLLER_DEFN_H
#define __WB_EV_SYS_EV_CONTROLLER_DEFN_H

#include "ev_controller_genetic_mapping.h"

#include "system_sim/controller_defn.h"

#include "params/dynamic_enum_schema.h"


namespace sys {
	namespace ev {

		class evolvable_controller_defn:
			public i_controller_defn
		{
			// i_controller_defn interface
		public:
			virtual std::string get_name() const override;
			virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;
			virtual controller_ptr create_controller(prm::param_accessor acc) const override;
		};

		class evolvable_controller_impl_defn
		{
		public:
			enum Components {
				CrossoverOp,
				MutationOp,
			};

			typedef std::function< i_system_defn const*(prm::param_accessor) > sys_defn_fn_t;

		public:
			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
			std::unique_ptr< i_genetic_mapping > generate(prm::param_accessor acc) const;

			evolvable_controller_impl_defn(sys_defn_fn_t sys_defn_fn);

		public:
			prm::dynamic_enum_schema< std::unique_ptr< i_genetic_mapping > > ev_controller_type_defn;
		};

	}
}



#endif


