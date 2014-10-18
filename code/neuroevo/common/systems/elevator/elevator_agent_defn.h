// elevator_agent_defn.h

#ifndef __WB_NE_ELEVATOR_AGENT_DEFN_H
#define __WB_NE_ELEVATOR_AGENT_DEFN_H

#include "system_sim/agent_defn.h"


namespace sys {
	namespace elev {

		class elevator_agent_defn:
			public i_agent_defn
		{
		public:
			virtual std::string get_name() const override;
			virtual std::string update_schema_providor_for_spec(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;
			virtual std::string update_schema_providor_for_instance(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;

			virtual state_value_id_list get_agent_state_values(prm::param_accessor acc) const override;
			virtual std::vector< std::string > sensor_inputs(prm::param_accessor acc) const override;
			virtual size_t num_effectors(prm::param_accessor acc) const override;

			virtual agent_ptr create_agent(prm::param_accessor spec_acc, prm::param_accessor inst_acc) const override;
		};

	}
}


#endif


