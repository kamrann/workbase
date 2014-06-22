// rtp_phys_resultant_objective.cpp

#include "rtp_phys_resultant_objective.h"
#include "rtp_phys_objectives.h"
#include "../../rtp_single_objective.h"
#include "../../rtp_pareto.h"

#include "wt_param_widgets/pw_yaml.h"


namespace YAML {
	template <>
	struct convert< i_population_wide_observer::Type >
	{
		static Node encode(i_population_wide_observer::Type const& rhs)
		{
			return Node(i_population_wide_observer::Names[rhs]);
		}

		static bool decode(Node const& node, i_population_wide_observer::Type& rhs)
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

		static std::map< std::string, i_population_wide_observer::Type > const mapping_;
	};

	std::map< std::string, i_population_wide_observer::Type > const convert< i_population_wide_observer::Type >::mapping_ = {
		{ "Single Objective", i_population_wide_observer::Type::Single },
		{ "Pareto Multiple Objective", i_population_wide_observer::Type::Pareto },
	};

	template <>
	struct convert< rtp_phys::agent_objective::Type >
	{
		static Node encode(rtp_phys::agent_objective::Type const& rhs)
		{
			return Node(rtp_phys::agent_objective::Names[rhs]);
		}

		static bool decode(Node const& node, rtp_phys::agent_objective::Type& rhs)
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

		static std::map< std::string, rtp_phys::agent_objective::Type > const mapping_;
	};

	std::map< std::string, rtp_phys::agent_objective::Type > const convert< rtp_phys::agent_objective::Type >::mapping_ = {
		{ "Max Final X", rtp_phys::agent_objective::Type::MaxFinalX },
		{ "Max Average Y", rtp_phys::agent_objective::Type::MaxAverageY },
		{ "Min Final Linear Speed", rtp_phys::agent_objective::Type::MinFinalLinearSpeed },
		{ "Min Average KE", rtp_phys::agent_objective::Type::MinAverageKE},
		{ "Maintain Altitude", rtp_phys::agent_objective::Type::MaintainAltitude },
	};
}

namespace rtp_phys
{
	namespace sb = prm::schema;

	YAML::Node resultant_objective::get_schema(YAML::Node const& param_vals)
	{
		auto schema = sb::list("objective");

		auto type = sb::enum_selection("Objective Type", { begin(i_population_wide_observer::Names), end(i_population_wide_observer::Names) });
		sb::on_update(type);
		sb::append(schema, type);

		auto node = prm::find_value(param_vals, "Objective Type");
		auto enum_sel = node.IsNull() ? i_population_wide_observer::Type::Default : node.as< i_population_wide_observer::Type >();
		switch(enum_sel)
		{
			case i_population_wide_observer::Type::Single:
			{
				auto type = sb::enum_selection("Objective", { begin(agent_objective::Names), end(agent_objective::Names) });
				//sb::on_update(type);
				sb::append(schema, type);
			}
			break;

			case i_population_wide_observer::Type::Pareto:
			{
				for(auto const& obj : agent_objective::Names)
				{
					sb::append(schema, sb::boolean(obj, false));
				}
			}
			break;

			default:
			assert(false);
		}

		return schema;
	}

	i_population_wide_observer* resultant_objective::create_instance(i_population_wide_observer::Type type, rtp_param param, std::set< agent_objective::Type >& required_observations)
	{
		switch(type)
		{
			case i_population_wide_observer::Single:
			{
				agent_objective::Type ao_type = boost::any_cast<agent_objective::Type>(param);
				required_observations.insert(ao_type);
				return new rtp_single_objective(agent_objective::Names[ao_type]);
			}

			case i_population_wide_observer::Pareto:
			{
				rtp_param_list param_list = boost::any_cast<rtp_param_list>(param);
				std::vector< std::string > obj_names;
				for(size_t i = 0; i < param_list.size(); ++i)
				{
					if(boost::any_cast<bool>(param_list[i]))
					{
						required_observations.insert((agent_objective::Type)i);
						obj_names.push_back(agent_objective::Names[i]);
					}
				}
				return new rtp_pareto(obj_names);
			}

			default:
			return nullptr;
		}
	}

	i_population_wide_observer* resultant_objective::create_instance(YAML::Node const& param, std::set< agent_objective::Type >& required_observations)
	{
		auto type = prm::find_value(param, "Objective Type").as< i_population_wide_observer::Type >();
		switch(type)
		{
			case i_population_wide_observer::Single:
			{
				auto ao_type = prm::find_value(param, "Objective").as< agent_objective::Type >();
				required_observations.insert(ao_type);
				return new rtp_single_objective(agent_objective::Names[ao_type]);
			}

			case i_population_wide_observer::Pareto:
			{
				std::vector< std::string > obj_names;
				for(auto const& obj : agent_objective::Names)
				{
					if(prm::find_value(param, obj).as< bool >())
					{
						required_observations.insert(YAML::Node(obj).as< agent_objective::Type >());
						obj_names.push_back(obj);
					}
				}
				return new rtp_pareto(obj_names);
			}

			default:
			return nullptr;
		}
	}
}


