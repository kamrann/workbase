// rtp_phys_scenario.cpp

#include "rtp_phys_scenario.h"
#include "scenarios/ground_based.h"
#include "scenarios/space_based.h"
#include "scenarios/target.h"
#include "scenarios/obstacles.h"
#include "../../params/realnum_par.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include <Wt/WComboBox>

#include <boost/math/constants/constants.hpp>


namespace rtp {

	const std::string phys_scenario_base::Names[] = {
		"Ground Based",
		"Space Based",
		"Target",
		"Obstacles",
	};

}

namespace YAML {
	template <>
	struct convert< rtp::phys_scenario_base::Type >
	{
		static Node encode(rtp::phys_scenario_base::Type const& rhs)
		{
			return Node(rtp::phys_scenario_base::Names[rhs]);
		}

		static bool decode(Node const& node, rtp::phys_scenario_base::Type& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp::phys_scenario_base::Type > const mapping_;
	};

	std::map< std::string, rtp::phys_scenario_base::Type > const convert< rtp::phys_scenario_base::Type >::mapping_ = {
		{ "Ground Based", rtp::phys_scenario_base::Type::GroundBased },
		{ "Space Based", rtp::phys_scenario_base::Type::SpaceBased },
		{ "Target", rtp::phys_scenario_base::Type::Target },
		{ "Obstacles", rtp::phys_scenario_base::Type::Obstacles },
	};
}

namespace rtp {

	namespace sb = prm::schema;

	std::string phys_scenario::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "scenario" };
		auto path = prefix + relative;

		(*provider)[path + std::string("world_type")] = [](prm::param_accessor)
		{
			auto s = sb::enum_selection(
				"world_type",
				std::vector< std::string >{ begin(Names), end(Names) },
				0, 1
			);
			sb::label(s, "World Type");
			sb::trigger(s, "scenario_type_changed");
			return s;
		};

		path += std::string{ "type_specific" };

		//auto ground = ground_based_scenario::update_schema_providor(provider, path);
		//auto space = space_based_scenario::update_schema_providor(provider, path);
		auto target = target_scenario::update_schema_providor(provider, path);
		auto obstacles = obstacles_scenario::update_schema_providor(provider, path);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("type_specific");
			//sb::label(s, "Scenario");
			auto node = param_vals["world_type"];
			auto enum_sel = (!node || node.as< prm::is_unspecified >()) ?
				Type::Default :
				node[0].as< Type >();
			switch(enum_sel)
			{
				case GroundBased:
				//sb::append(s, provider->at(path + ground)(param_vals));
				break;

				case SpaceBased:
				//sb::append(s, provider->at(path + space)(param_vals));
				break;

				case Target:
				sb::append(s, provider->at(path + target)(param_vals));
				break;

				case Obstacles:
				sb::append(s, provider->at(path + obstacles)(param_vals));
				break;

				default:
				break;
			}
			sb::update_on(s, "scenario_type_changed");
			return s;
		};

		path.pop();

		(*provider)[path + std::string("duration")] = [](prm::param_accessor)
		{
			auto s = sb::real(
				"duration",
				10.0,
				0.0,
				1000.0
				);
			sb::label(s, "Duration");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::label(s, "Scenario");
			sb::append(s, provider->at(path + std::string("world_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("type_specific"))(param_vals));
			sb::append(s, provider->at(path + std::string("duration"))(param_vals));
			sb::enable_import(s, "physics2d.scenario");
			return s;
		};

		return relative;
	}

	std::unique_ptr< phys_scenario > phys_scenario::create_instance(prm::param_accessor param)
	{
		std::unique_ptr< phys_scenario > scenario;

		param.push_relative_path(prm::qualified_path("scenario"));
		auto type_node = param["world_type"];
		if(type_node.as< prm::is_unspecified >())
		{
			throw prm::required_unspecified("World Type");
		}
		
		Type scenario_type = type_node[0].as< Type >();
		switch(scenario_type)
		{
			case GroundBased:
			scenario = std::make_unique< ground_based_scenario >();
			break;

			case SpaceBased:
			scenario = std::make_unique< space_based_scenario >();
			break;

			case Target:
			scenario = std::make_unique< target_scenario >(param);
			break;

			case Obstacles:
			scenario = std::make_unique< obstacles_scenario >(param);
			break;

			default:
			assert(false);
		}

		scenario->m_duration = param["duration"].as< double >();

		param.pop_relative_path();
		return scenario;
	}

	std::vector< std::string > phys_scenario::get_state_values(prm::param_accessor param_vals)
	{
		auto scenario_type = param_vals["world_type"][0].as< phys_scenario::Type >();
		switch(scenario_type)
		{
			case phys_scenario::Obstacles:
			return obstacles_scenario::get_state_values(param_vals);

			case phys_scenario::GroundBased:
			case phys_scenario::SpaceBased:
			case phys_scenario::Target:
			return{};

			default:
			throw std::exception("Invalid Scenario Type");
		}
	}


	phys_scenario::scenario_data_t phys_scenario::get_scenario_data() const
	{
		return scenario_data_t();
	}

	bool phys_scenario::is_complete(state_t const& st)
	{
		return st.time >= m_duration;
	}
}


