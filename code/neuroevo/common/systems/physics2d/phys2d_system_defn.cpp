// phys2d_system_defn.cpp

#include "phys2d_system_defn.h"
#include "phys2d_systemstatevalues.h"
#include "phys2d_system.h"

#include "ground_scenario_defn.h"	// for create_default()
#include "basic_biped_defn.h"		// for create_default()

#include "system_sim/system_update_info.h"
#include "system_sim/controller.h"
#include "system_sim/passive_controller.h"	// for create_default()

#include "params/param_accessor.h"
#include "params/param_yaml.h"
#include "params/schema_builder.h"


namespace sys {
	namespace phys2d {

		const bimap< phys2d_system_defn::StateValue, std::string > phys2d_system_defn::s_state_values = {
				{ StateValue::Time, "Time" },
		};


		std::unique_ptr< phys2d_system_defn > phys2d_system_defn::create_default()
		{
			auto defn = std::make_unique< phys2d_system_defn >();

			defn->add_scenario_defn(std::make_unique< ground_scenario_defn >());
			defn->add_agent_defn("default", std::make_unique< basic_biped_defn >());
			defn->add_controller_defn("preset", std::make_unique< passive_controller_defn >());

			return defn;
		}


		std::string phys2d_system_defn::get_name() const
		{
			return "physics_2d";
		}

		bool phys2d_system_defn::is_instantaneous() const
		{
			return false;
		}

		update_info phys2d_system_defn::get_update_info() const
		{
			auto inf = update_info{};
			inf.type = update_info::Type::Realtime;
			inf.frequency = 0; // TODO: problem m_hertz;
			return inf;
		}

		void phys2d_system_defn::add_scenario_defn(scenario_defn_ptr defn)
		{
			auto name = defn->get_name();
			m_scenario_defns[name] = std::move(defn);
		}

		namespace sb = prm::schema;

		std::string phys2d_system_defn::update_schema_provider_for_system_core(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

			(*provider)[path + std::string("updates_per_sec")] = [](prm::param_accessor)
			{
				auto s = sb::integer("updates_per_sec", 60, 10, 1000);
				sb::label(s, "Updates/Sec");
				return s;
			};

			path += std::string{ "scenario" };

			(*provider)[path + std::string{ "scenario_type" }] = [=](prm::param_accessor acc)
			{
				auto scenario_names = std::vector < std::string > {};
				for(auto const& scen : m_scenario_defns)
				{
					scenario_names.push_back(scen.second->get_name());
				}

				auto s = sb::enum_selection(
					"scenario_type",
					scenario_names,
					0,
					1
					);
				return s;
			};

			// Store the schema provider components for every registered scenario type
			for(auto const& entry : m_scenario_defns)
			{
				auto const& scen = entry.second;
				scen->update_schema_providor(provider, path + scen->get_name());
			}

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string("scenario_type"))(acc));
				if(acc.is_available("scenario_type") && !prm::is_unspecified(acc["scenario_type"]))
				{
					auto scen_name = prm::extract_as< prm::enum_param_val >(acc["scenario_type"])[0];
					sb::append(s, provider->at(path + scen_name)(acc));
				}
				return s;
			};

			path.pop();

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string("updates_per_sec"))(acc));
				sb::append(s, provider->at(path + std::string{ "scenario" })(acc));

				sb::border(s, std::string{ "Physics 2D" });
				sb::enable_import(s, "system.physics2d");
				return s;
			};

			return path.leaf().name();
		}


		state_value_id_list phys2d_system_defn::get_system_core_state_values(prm::param_accessor acc) const
		{
			state_value_id_list sv_names;
			for(auto const& sv : s_state_values.left)
			{
				sv_names.push_back(state_value_id{ sv.second });
			}
			return sv_names;
		}

		system_ptr phys2d_system_defn::create_system_core(prm::param_accessor acc) const
		{
			// Store param tree within defn, or externally? ex
			// Have separate system factory, or incorporate into defn? inc

			auto update_rate = prm::extract_as< int >(acc["updates_per_sec"]);

			acc.move_to(acc.find_path("scenario"));
			auto scenario_name = prm::extract_as< prm::enum_param_val >(acc["scenario_type"])[0];
			auto scen = m_scenario_defns.at(scenario_name)->create_scenario(acc);

			auto sys = std::make_unique< phys2d_system >(std::move(scen), update_rate);
			return std::move(sys);
		}

	}
}



