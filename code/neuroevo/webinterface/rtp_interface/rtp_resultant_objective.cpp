// rtp_resultant_objective.cpp

#include "rtp_resultant_objective.h"
#include "rtp_single_objective.h"
#include "rtp_pareto.h"
#include "rtp_null_objective.h"
#include "rtp_manual_objective.h"
#include "systems/rtp_system.h"
#include "systems/rtp_agent.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"


namespace YAML {

	template <>
	struct convert< rtp::i_population_wide_observer::Type >
	{
		static Node encode(rtp::i_population_wide_observer::Type const& rhs)
		{
			return Node(rtp::i_population_wide_observer::Names[rhs]);
		}

		static bool decode(Node const& node, rtp::i_population_wide_observer::Type& rhs)
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

		static std::map< std::string, rtp::i_population_wide_observer::Type > const mapping_;
	};

	std::map< std::string, rtp::i_population_wide_observer::Type > const convert< rtp::i_population_wide_observer::Type >::mapping_ = {
			{ "Single Objective", rtp::i_population_wide_observer::Type::Single },
			{ "Pareto Multiple Objective", rtp::i_population_wide_observer::Type::Pareto },
			{ "Null", rtp::i_population_wide_observer::Type::Null_Debug },
	};
/*
	template <>
	struct convert< rtp::agent_objective::Type >
	{
		static Node encode(rtp::agent_objective::Type const& rhs)
		{
			return Node(rtp::agent_objective::Names[rhs]);
		}

		static bool decode(Node const& node, rtp::agent_objective::Type& rhs)
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

		static std::map< std::string, rtp::agent_objective::Type > const mapping_;
	};

	std::map< std::string, rtp::agent_objective::Type > const convert< rtp::agent_objective::Type >::mapping_ = {
		{ "Max Final X", rtp::agent_objective::Type::MaxFinalX },
		{ "Max Average Y", rtp::agent_objective::Type::MaxAverageY },
		{ "Min Final Linear Speed", rtp::agent_objective::Type::MinFinalLinearSpeed },
		{ "Min Average KE", rtp::agent_objective::Type::MinAverageKE},
		{ "Maintain Altitude", rtp::agent_objective::Type::MaintainAltitude },
		{ "Min Non-Foot Contacts", rtp::agent_objective::Type::MinNonFootContacts },
	};
	*/
}

namespace rtp
{
	namespace sb = prm::schema;

	std::string resultant_objective::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "objective" };
		auto path = prefix + relative;

		(*provider)[path + std::string("obj_type")] = [](prm::param_accessor)
		{
			auto s = sb::enum_selection("obj_type", std::vector< std::string >{ begin(i_population_wide_observer::Names), end(i_population_wide_observer::Names) });
			sb::label(s, "Objective Type");
			sb::trigger(s, "obj_type");
			return s;
		};

		path += std::string("objective_components");

		// TODO: This seems weird, having to register twice due to be used in multiple points in the hierarchy
		auto single_manualobj_rel = manual_objective::update_schema_providor(provider, path);

		(*provider)[path + std::string("trial_merging")] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::enum_selection(
				"trial_merging",
				std::vector< std::string >{ "Average", "Worst" },
				1, 1);
			sb::label(s, "Trial Merging");
			return s;
		};

		// TODO: Not too happy about this. Now changed to providing repeat contents as id name only, the schema
		// accessor is required to be registered relative to the repeat. Maybe better to provide a fully 
		// qualified path for the contents.
		auto pareto_manualobj_rel = manual_objective::update_schema_providor(provider, path + std::string("pareto_components"));

		(*provider)[path + std::string("pareto_components")] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::repeating_list(
				"pareto_components",
				//provider->at(path + pareto_manualobj_rel)(param_vals),
				pareto_manualobj_rel,
				0,
				0);
			sb::unborder(s);
			return s;
		};


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("objective_components");
			auto node = param_vals["obj_type"];
			auto enum_sel = !node ? i_population_wide_observer::Type::Default : node[0].as< i_population_wide_observer::Type >();
			switch(enum_sel)
			{
				case i_population_wide_observer::Type::Single:
				sb::append(s, provider->at(path + single_manualobj_rel)(param_vals));
				sb::append(s, provider->at(path + std::string{ "trial_merging" })(param_vals));
				break;

				case i_population_wide_observer::Type::Pareto:
				sb::append(s, provider->at(path + std::string("pareto_components"))(param_vals));
				break;

				case i_population_wide_observer::Type::Null_Debug:
				break;
			}
			sb::unborder(s);
			sb::update_on(s, "obj_type");
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("obj_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("objective_components"))(param_vals));
			return s;
		};

		return relative;
	}

	class temp_manual_objective_wrapper: public i_observer
	{
	public:
		temp_manual_objective_wrapper(std::unique_ptr< manual_objective > mo, std::string const& observation_name):
			m_man_obj(std::move(mo)),
			m_observation_name(observation_name)
		{

		}

	public:
		virtual void reset() override
		{
			m_man_obj->reset();
		}

		virtual void update(i_system const* system) override
		{
			m_man_obj->set_stateval_fn([system](state_value_id id)
			{
				if(id.find("::sys") == 0)
				{
					static const size_t off = std::string("::system::").length();
					auto sv_id = id.substr(off);
					return system->get_state_value(sv_id);
				}
				else
				{
					static const size_t off = std::string("::agent::").length();
					auto sensor_str = id.substr(off);
					auto sensor_idx = std::stoi(sensor_str);
					return system->get_agent(0 // !!!!!
						).get_sensor_value(sensor_idx);
				}
			});

			m_man_obj->update();
		}

		virtual i_observer::observations_t record_observations(trial_data_t const& td) override
		{
			m_man_obj->finalise();
			i_observer::observations_t obs;
			auto obj_val = m_man_obj->get_objective_value();
			assert(obj_val);
			obs[m_observation_name] = *obj_val;
			return obs;
		}

	protected:
		std::unique_ptr< manual_objective > m_man_obj;
		std::string m_observation_name;
	};

	std::unique_ptr< i_population_wide_observer > resultant_objective::create_instance(
		prm::param_accessor param,
		std::vector< std::unique_ptr< i_observer > >& required_observations)
		//std::set< agent_objective::Type >& required_observations)
	{
#if 0
		auto type = prm::find_value(param, "obj_type")[0].as< i_population_wide_observer::Type >();
		switch(type)
		{
			case i_population_wide_observer::Single:
			{
				auto ao_type = prm::find_value(param, "single_obj")[0].as< agent_objective::Type >();
				required_observations.insert(ao_type);
				return new rtp_single_objective(agent_objective::Names[ao_type]);
			}

			case i_population_wide_observer::Pareto:
			{
				auto ep = prm::find_value(param, "pareto_obj").as< prm::enum_param_val >();
				auto objectives = boost::any_cast<std::vector< std::string >>(ep.contents);
				for(auto const& obj_name : objectives)
				{
					required_observations.insert(YAML::Node(obj_name).as< agent_objective::Type >());
				}

				/*				std::vector< std::string > obj_names;
								for(auto const& obj : agent_objective::Names)
								{
								if(prm::find_value(param, obj).as< bool >())
								{
								required_observations.insert(YAML::Node(obj).as< agent_objective::Type >());
								obj_names.push_back(obj);
								}
								}
								*/
				return new rtp_pareto(objectives);// obj_names);
			}

			default:
			return nullptr;
		}
#endif

		param.push_relative_path(prm::qualified_path("objective"));
		auto obj_type = param["obj_type"][0].as< i_population_wide_observer::Type >();
		param.push_relative_path(prm::qualified_path("objective_components"));
		std::unique_ptr< i_population_wide_observer > result;
		switch(obj_type)
		{
			case i_population_wide_observer::Type::Single:
			{
				auto man_obj = manual_objective::create_instance(param, std::function< double(state_value_id) >());	// TODO:
				auto observation_name = std::string("SINGLE");
				auto wrapped = std::make_unique< temp_manual_objective_wrapper >(std::move(man_obj), observation_name);
				required_observations.emplace_back(std::move(wrapped));

				// TODO:
				auto merge = param["trial_merging"][0].as< std::string >();
				rtp_single_objective::MergeMethod merge_method = rtp_single_objective::MergeMethod::Default;
				if(merge == "Average")
				{
					merge_method = rtp_single_objective::MergeMethod::Average;
				}
				else
				{
					merge_method = rtp_single_objective::MergeMethod::Minimum;
				}

				result = std::make_unique< rtp_single_objective >(observation_name, merge_method);
			}
			break;

			case i_population_wide_observer::Type::Pareto:
			{
				std::vector< std::string > components;
				auto node = param["pareto_components"];
				//assert(node[prm::ParamNode::Value] && node[prm::ParamNode::Value].IsMap());
				assert(node.IsMap());
				for(auto inst : node)//[prm::ParamNode::Value])
				{
					auto inst_num = inst.first.as< unsigned int >();
					auto rel_path = prm::qualified_path("pareto_components");
					rel_path.leaf().set_index(inst_num);
					param.push_relative_path(rel_path);
					auto man_obj = manual_objective::create_instance(param, std::function< double(state_value_id) >());	// TODO: ?? Currently set in every update() call, so this is not used...
					// TODO: For now just using unique path as observation name
					std::stringstream ss;
					ss << rel_path;
					auto observation_name = ss.str();
					auto wrapped = std::make_unique< temp_manual_objective_wrapper >(std::move(man_obj), observation_name);
					required_observations.emplace_back(std::move(wrapped));
					param.pop_relative_path();
					components.push_back(observation_name);
				}
				result = std::make_unique< rtp_pareto >(components);
			}
			break;

			case i_population_wide_observer::Type::Null_Debug:
			{
				result = std::make_unique< rtp_null_objective >();
			}
			break;
		}
		param.pop_relative_path();
		param.pop_relative_path();

		return result;
	}
}


