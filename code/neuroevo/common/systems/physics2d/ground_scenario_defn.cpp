// ground_scenario_defn.cpp

#include "ground_scenario_defn.h"
#include "ground_scenario.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace sys {
	namespace phys2d {

		namespace sb = prm::schema;

		std::string ground_scenario_defn::get_name() const
		{
			return "ground_based";
		}

		std::string ground_scenario_defn::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

			(*provider)[path + std::string("ground_expanse")] = [](prm::param_accessor)
			{
				auto s = sb::real(
					"ground_expanse",
					10.0,
					0.0
					);
				sb::label(s, "Ground Expanse");
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string{ "ground_expanse" })(acc));
				return s;
			};

			return path.leaf().name();
		}

		state_value_id_list ground_scenario_defn::get_state_values(prm::param_accessor acc) const
		{
			return{};
		}

		std::unique_ptr< scenario > ground_scenario_defn::create_scenario(prm::param_accessor acc) const
		{
			spec_data spec;
			spec.expanse = prm::extract_as< double >(acc["ground_expanse"]);
			return std::make_unique< ground_scenario >(spec);
		}

	}
}


