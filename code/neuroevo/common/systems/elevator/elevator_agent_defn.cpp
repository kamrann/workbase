// elevator_agent_defn.cpp

#include "elevator_agent_defn.h"
#include "elevator_agent.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace sb = prm::schema;

namespace sys {
	namespace elev {

		std::string elevator_agent_defn::get_name() const
		{
			return "the_elevator";
		}

		std::string elevator_agent_defn::update_schema_providor_for_spec(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				return s;
			};

			return path.leaf().name();
		}

		std::string elevator_agent_defn::update_schema_providor_for_instance(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				return s;
			};

			return path.leaf().name();
		}

		state_value_id_list elevator_agent_defn::get_agent_state_values(prm::param_accessor acc) const
		{
			return{}; // TODO:
		}

		std::vector< std::string > elevator_agent_defn::sensor_inputs(prm::param_accessor acc) const
		{
			return{}; // TODO:
		}

		size_t elevator_agent_defn::num_effectors(prm::param_accessor acc) const
		{
			// 1: Up/Down/Remain = 1/-1/0
			// 2: Open/Don't open = 1/0
			return 2;
		}

		agent_ptr elevator_agent_defn::create_agent(prm::param_accessor spec_acc, prm::param_accessor inst_acc) const
		{
			return std::make_unique< elevator_agent >();
		}

	}
}


