// ffnn_ev_controller_defn.h

#ifndef __WB_EV_SYS_FFNN_EV_CONTROLLER_DEFN_H
#define __WB_EV_SYS_FFNN_EV_CONTROLLER_DEFN_H

#include "ev_controller_genetic_mapping.h"
#include "evolved_controller_defn.h"

#include "params/param_fwd.h"
#include "params/dynamic_enum_schema.h"


namespace sys {
	namespace ev {

		class ffnn_ev_controller_defn
		{
		public:
			ffnn_ev_controller_defn(evolvable_controller_impl_defn::sys_defn_fn_t sys_defn_fn);

			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, int component);
			std::unique_ptr< i_genetic_mapping > generate(prm::param_accessor acc) const;

		public:
			evolvable_controller_impl_defn::sys_defn_fn_t sys_defn_fn_;

			typedef ga::gene_mutation< double > gene_mut_fn_t;
			prm::dynamic_enum_schema< gene_mut_fn_t > gene_mutation_defn_;
		};

	}
}



#endif


