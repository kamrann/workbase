// rtp_phys_controller.cpp

#include "rtp_phys_controller.h"
#include "controllers/passive.h"
#include "controllers/interactive.h"
#include "controllers/spaceship_interactive.h"
#include "controllers/test_controller.h"

#include "../rtp_system_agents_data.h"
#include "../../rtp_genome_mapping.h"

#include "../../../webinterfaceapp.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"


namespace rtp
{
	std::string const i_phys_controller::Names[] = {
		"Evolved",
		"Passive",
		"Interactive",
		"Database",
	};
}

namespace YAML {

	std::map< std::string, rtp::i_phys_controller::Type > const convert< rtp::i_phys_controller::Type >::mapping_ = {
		{ "Evolved", rtp::i_phys_controller::Type::Evolved },
		{ "Passive", rtp::i_phys_controller::Type::Passive },
		{ "Interactive", rtp::i_phys_controller::Type::Interactive },
		{ "Database", rtp::i_phys_controller::Type::Database },
	};
}

namespace rtp {

	prm::param i_phys_controller::get_controller_agent_type(prm::param_accessor param)
	{
		auto spec_id = param["spec_reference"][0].as< std::string >();
		prm::qualified_path spec_list_path = param.find_path("agent_spec_list");
		auto spec_list_node = param["agent_spec_list"];
		boost::optional< prm::qualified_path > spec_path;
		for(auto entry : spec_list_node)
		{
			auto inst_num = entry.first.as< unsigned int >();
			spec_list_path.leaf().set_index(inst_num);

			auto name = param.from_absolute(spec_list_path)["name"].as< std::string >();
			if(name == spec_id)
			{
				spec_path = spec_list_path;
				break;
			}
		}
		if(!spec_path)
		{
			throw std::exception("Invalid Spec Reference");
		}

		return param.from_absolute(*spec_path)["spec_type"][0];
	}
	
	std::vector< std::pair< prm::qualified_path, std::string > > get_compatible_evolvable_controllers(
		agent_body_spec::Type agent_type,
		prm::param_accessor param_vals)
	{
		std::vector< std::pair< prm::qualified_path, std::string > > controllers;

		auto node = param_vals["evolved_list"];
		auto evolved_list_path =
			//resolve_id("evolved_list", param_vals.get_root(), param_vals.get_current_path(), param_vals.get_all_paths());
			param_vals.find_path("evolved_list");
		for(auto evolved_entry : node)
		{
			auto inst_num = evolved_entry.first.as< unsigned int >();
			evolved_list_path.leaf().set_index(inst_num);

			auto compatible_node = param_vals.from_absolute(evolved_list_path)["compatible_agent_types"];
			bool type_unspecified = compatible_node.as< prm::is_unspecified >();
			if(type_unspecified)
			{
				continue;
			}
			auto controller_agent_type = compatible_node[0].as< agent_body_spec::Type >();
			if(controller_agent_type != agent_type)
			{
				continue;
			}

			auto name = param_vals.from_absolute(evolved_list_path)["name"].as< std::string >();

			controllers.emplace_back(std::make_pair(
				evolved_list_path + std::string{ "evolved" },
				name
				));
		}

		return controllers;
	}

	namespace sb = prm::schema;

	boost::optional< std::string > update_extended_interactive_schema_provider(
		prm::schema::schema_provider_map_handle provider,
		prm::qualified_path const& prefix,
		agent_body_spec::Type agent_type)
	{
		switch(agent_type)
		{
			case agent_body_spec::Type::Spaceship:
			break;

			default:
			return boost::none;
		}
	}

	std::string i_phys_controller::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable)
	{
		auto relative = std::string{ "controller_params" };
		auto path = prefix + relative;

		(*provider)[path + std::string("controller_type")] = [evolvable](prm::param_accessor)
		{
			auto names_begin = evolvable ? begin(Names) : (begin(Names) + 1);
			auto s = sb::enum_selection(
				"controller_type",
				std::vector< std::string >{ names_begin, end(Names) },
				0, 1
			);
			sb::label(s, "Controller Type");
			sb::trigger(s, "controller_type_changed");
			return s;
		};


		path += std::string("controller_type_specific_params");

		(*provider)[path + std::string("evolved_controller_sel")] = [=](prm::param_accessor param_vals)
		{
			auto agent_type_node = get_controller_agent_type(param_vals);
			auto agent_type = agent_type_node.as< agent_body_spec::Type >();
			auto controllers = get_compatible_evolvable_controllers(agent_type, param_vals);
			auto num_available = controllers.size();
			std::vector< std::pair< std::string, std::string > > elements(num_available);
			int idx = 0;
			std::transform(
				begin(controllers),
				end(controllers),
				begin(elements),
				[](std::pair< prm::qualified_path, std::string > const& in)
			{
				return std::make_pair(
					//in.first.to_string(),
					in.second,
					in.second
					);
			});

			auto s = sb::enum_selection("evolved_controller_sel", elements, 0, 1);
			sb::unlabel(s);
			sb::update_on(s, "$evolved_controller_added");
			sb::update_on(s, "$evolved_controller_removed");
			sb::update_on(s, "$evolved_controller_compatibility_changed");
			return s;
		};

		(*provider)[path + std::string("db_controller")] = [=](prm::param_accessor param_vals)
		{
			// TODO: currently retrieving list from database here and storing in schema.
			// alternative would be to have schema support a database query select control,
			// and then just pass query string as its parameter.

			auto app = (WebInterfaceApplication*)Wt::WApplication::instance();
			evodb_session db_s(*app->db_cp);
			dbo::Transaction t(db_s);
			auto query = db_s.find< named_genome >();
			auto results = query.resultList();

			std::vector< std::string > entries;
			for(auto row : results)
			{
				entries.emplace_back(row->name);
			}
			t.commit();

			auto s = sb::enum_selection("db_controller", entries, 0, 1);
			sb::unlabel(s);
			return s;
		};

		auto direct_bindings_rel = i_interactive_controller::update_direct_bindings_schema_providor(provider, path);
		
		auto spaceship_bindings_rel = spaceship_interactive_controller::update_schema_provider(provider, path);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("controller_type_specific_params");

			auto node = param_vals["controller_type"];
			auto enum_sel = (!node || node.as< prm::is_unspecified >()) ?
				Type::Default :
				node[0].as< Type >();
			switch(enum_sel)
			{
				case Evolved:
				sb::append(s, provider->at(path + std::string("evolved_controller_sel"))(param_vals));
				break;

				case Database:
				sb::append(s, provider->at(path + std::string("db_controller"))(param_vals));
				break;

				case Interactive:
				{
					sb::append(s, provider->at(path + direct_bindings_rel)(param_vals));

					auto agent_type = get_controller_agent_type(param_vals).as< agent_body_spec::Type >();
					switch(agent_type)
					{
						case agent_body_spec::Type::Spaceship:
						sb::append(s, provider->at(path + spaceship_bindings_rel)(param_vals));
						break;
					}
//					sb::append(s, provider->at(path + std::string("interactive_controller_type"))(param_vals));
//					auto type = param_vals["interactive_controller_type"]
				}
				break;

				default:
				break;
			}
			
			sb::unborder(s);
			sb::update_on(s, "controller_type_changed");
			return s;
		};

		path.pop();


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("controller_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("controller_type_specific_params"))(param_vals));
			return s;
		};

		return relative;
	}

	class db_controller_factory: public i_controller_factory
	{
	public:
		db_controller_factory(
			std::unique_ptr< i_controller_factory > base_factory,
			std::unique_ptr< i_genome_mapping > mapping,
			std::unique_ptr< i_genome > genome
			):
			m_base_factory(std::move(base_factory)),
			m_mapping(std::move(mapping)),
			m_genome(std::move(genome))
		{}

		virtual std::unique_ptr< i_controller > create(i_agent const* agent) const override
		{
			auto controller = m_base_factory->create(agent);
			m_mapping->decode_genome(m_genome.get(), controller.get());
			return controller;
		}

		virtual bool is_interactive() const override
		{
			return false;
		}

	private:
		std::unique_ptr< i_controller_factory > m_base_factory;
		std::unique_ptr< i_genome_mapping > m_mapping;
		std::unique_ptr< i_genome > m_genome;
	};

	std::unique_ptr< i_controller_factory > i_phys_controller::create_factory(prm::param_accessor param)
	{
		std::unique_ptr< i_controller_factory > result;
		param.push_relative_path(prm::qualified_path("controller_params"));

		auto controller_type = param["controller_type"][0].as< Type >();
		switch(controller_type)
		{
			case Type::Passive:
			result = std::make_unique< controller_factory >([](i_agent const*) -> std::unique_ptr< i_controller >
			{
				return std::make_unique< passive_controller >();
			});
			break;

			case Type::Interactive:
			{
				auto agent_type = get_controller_agent_type(param).as< agent_body_spec::Type >();
				switch(agent_type)
				{
					case agent_body_spec::Type::Spaceship:
					result = spaceship_interactive_controller::create_factory(param);
					break;

					default:
					result = i_interactive_controller::create_factory(param);
					break;
				}
			}
			break;

			case Type::Database:
			{
				// TODO: Need schema enum to support separate label and value strings, then use the 
				// value string to store database row id.
				auto name = param["db_controller"][0].as< std::string >();

				auto app = (WebInterfaceApplication*)Wt::WApplication::instance();
				evodb_session db_s(*app->db_cp);
				dbo::Transaction t(db_s);
				auto query = db_s.find< named_genome >().where("name = ?").bind(name);
				auto query_result = query.resultValue();

				auto pgn = query_result->gn;
				auto pevo_run = pgn->generation->run;
				auto run_params = YAML::Load(pevo_run->sys_params);

				// TODO: Need to change this, only want to generate genome mapping and controller factory, nothing else is needed!
				prm::param_accessor run_params_acc(run_params);
				run_params_acc.push_relative_path(std::string{ "sim_params" });
				auto sys_factory = i_system::generate_system_factory(run_params_acc);
				auto agent_data = i_system::generate_agents_data(run_params_acc, true);
				run_params_acc.pop_relative_path();

				// TODO: Which one ??????????????
				auto controller_id = agent_data.agents.front().controller_id;
				//
//				auto c_factory = std::move(agent_data.controller_factories[controller_id]);
//				auto gn_mapping = std::move(agent_data.evolved[controller_id].genome_mapping);
				auto c_factory = std::move(agent_data.controller_factories.at(controller_id));
				auto gn_mapping = std::move(agent_data.evolved.at(controller_id).genome_mapping);
				//
				auto gn = gn_mapping->load_genome_from_binary(pgn->encoding);

				t.commit();

				result = std::make_unique< db_controller_factory >(
					std::move(c_factory),
					std::move(gn_mapping),
					std::move(gn)
					);
			}
			break;

			default:
			return nullptr;
		}

		param.pop_relative_path();
		return result;
	}

#if 0
	i_phys_controller* i_phys_controller::create_instance(prm::param_accessor param)
	{
		param.push_relative_path(prm::qualified_path("controller_params"));

		i_phys_controller* controller = nullptr;
		auto type = param["controller_type"][0].as< Type >();

		param.push_relative_path(prm::qualified_path("controller_type_specific_params"));		
		switch(type)
		{
			case Evolved:
			controller = nullptr;
			break;

			case Passive:
			controller = new passive_controller(/*sub*/);
			break;

			case Interactive:
			controller = new interactive_controller(/*sub*/);
			break;

			case Test:
			controller = new test_controller();
			break;

			case Database:
			{
				// TODO: Need schema enum to support separate label and value strings, then use the 
				// value string to store database row id.
				auto name = param["named_controller"][0].as< std::string >();

				auto app = (WebInterfaceApplication*)Wt::WApplication::instance();
				evodb_session db_s(*app->db_cp);
				dbo::Transaction t(db_s);
				auto query = db_s.find< named_genome >().where("name = ?").bind(name);
				auto result = query.resultValue();

				auto pgn = result->gn;
				auto pevo_run = pgn->generation->run;
				auto run_params = YAML::Load(pevo_run->sys_params);
#if 0 TODO:
				// TODO: Need to change this, only want to generate genome mapping and agent factory, nothing else is needed!
				prm::param_accessor run_params_acc(run_params);
				auto interfaces = i_system::create_instance(run_params_acc, true);
				delete std::get< 0 >(interfaces);
				delete std::get< 3 >(interfaces);
				delete std::get< 4 >(interfaces);
				auto gn_mapping = std::get< 1 >(interfaces);
				auto a_factory = std::get< 2 >(interfaces);
				//

				auto agent = a_factory->create();
				auto gn = gn_mapping->load_genome_from_binary(pgn->encoding);
				gn_mapping->decode_genome(gn, agent);

				t.commit();
				delete gn_mapping;
				delete a_factory;
				delete gn;

				controller = static_cast<i_phys_controller*>(agent);
#endif
			}
			break;
		}

		param.pop_relative_path();
		param.pop_relative_path();
		return controller;
	}

	i_phys_controller* i_phys_controller::create_instance(prm::param& param)
	{
		i_phys_controller* controller = nullptr;
		auto type = param["type"].as< Type >();

		switch(type)
		{
			case Evolved:
			controller = nullptr;
			break;

			case Passive:
			controller = new passive_controller(/*sub*/);
			break;

			case Interactive:
			controller = new interactive_controller(/*sub*/);
			break;

			case Test:
			controller = new test_controller();
			break;

			case Database:
			{
				// TODO: Need schema enum to support separate label and value strings, then use the 
				// value string to store database row id.
				auto name = param["named_controller"].as< std::string >();

				auto app = (WebInterfaceApplication*)Wt::WApplication::instance();
				evodb_session db_s(*app->db_cp);
				dbo::Transaction t(db_s);
				auto query = db_s.find< named_genome >().where("name = ?").bind(name);
				auto result = query.resultValue();

				auto pgn = result->gn;
				auto pevo_run = pgn->generation->run;
				auto run_params = YAML::Load(pevo_run->sys_params);
#if 0 TODO:
				// TODO: Need to change this, only want to generate genome mapping and agent factory, nothing else is needed!
				//prm::param_accessor run_params_acc(run_params);
				auto interfaces = i_system::create_instance(run_params, true);
				delete std::get< 0 >(interfaces);
				delete std::get< 3 >(interfaces);
				delete std::get< 4 >(interfaces);
				auto gn_mapping = std::get< 1 >(interfaces);
				auto a_factory = std::get< 2 >(interfaces);
				//

				auto agent = a_factory->create();
				auto gn = gn_mapping->load_genome_from_binary(pgn->encoding);
				gn_mapping->decode_genome(gn, agent);

				t.commit();
				delete gn_mapping;
				delete a_factory;
				delete gn;

				controller = static_cast<i_phys_controller*>(agent);
#endif
			}
			break;
		}

		return controller;
	}
#endif
}


