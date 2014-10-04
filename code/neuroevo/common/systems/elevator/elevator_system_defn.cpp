// elevator_system_defn.cpp

#include "elevator_system_defn.h"
#include "elevator_system.h"

#include "../../system_sim/system_update_info.h"
#include "../../system_sim/rtp_controller.h"

#include "params/param_accessor.h"
#include "params/param_yaml.h"
#include "params/schema_builder.h"


namespace sys {
	namespace elev {

		bool elevator_system_defn::is_instantaneous() const
		{
			return false;
		}

		update_info elevator_system_defn::get_update_info() const
		{
			auto inf = update_info{};
			inf.type = update_info::Type::Nonrealtime;
			return inf;
		}

		namespace sb = prm::schema;

		std::string elevator_system_defn::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto relative = std::string{ "elevator_params" };
			auto path = prefix + relative;

			(*provider)[path + std::string("floor_count")] = [](prm::param_accessor)
			{
				auto s = sb::integer("floor_count", 2, 2);
				sb::label(s, "Floors");
				return s;
			};

			(*provider)[path + std::string("arrival_rate")] = [](prm::param_accessor)
			{
				auto s = sb::real("arrival_rate", 0.1, 0.0);
				sb::label(s, "Arrival Rate");
				return s;
			};

			auto controller_rel = update_schema_provider_for_agent_controller(provider, path);

			(*provider)[path] = [=](prm::param_accessor param_vals)
			{
				auto s = sb::list(relative);
				sb::append(s, provider->at(path + std::string("floor_count"))(param_vals));
				sb::append(s, provider->at(path + std::string("arrival_rate"))(param_vals));

				sb::append(s, provider->at(path + controller_rel)(param_vals));

				sb::border(s, std::string{ "Elevator" });
				sb::enable_import(s, "system.elevator");
				return s;
			};

			return relative;
		}

		std::vector< std::string > elevator_system_defn::get_agent_type_names() const
		{
			return{ "The Elevator" };
		}

		std::vector< std::string > elevator_system_defn::get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const
		{
			// TODO:
			auto const num_floors = prm::extract_as< floor_count_t >(param_vals["floor_count"]);
			return generate_state_value_names(num_floors);
		}

		size_t elevator_system_defn::get_agent_num_effectors(prm::param agent_type) const
		{
			// 1: Up/Down/Remain = 1/-1/0
			// 2: Open/Don't open = 1/0
			return 2;
		}

		std::vector< std::string > elevator_system_defn::get_system_state_values(prm::param_accessor acc) const
		{
			auto num_floors = prm::extract_as< floor_count_t >(acc["floor_count"]);
			return generate_state_value_names(num_floors);
		}

		system_ptr elevator_system_defn::create_system(prm::param_accessor acc) const
		{
			// Store param tree within defn, or externally? ex
			// Have separate system factory, or incorporate into defn? inc

			auto sys = std::make_unique< elevator_system >(acc);

			auto controller = create_controller(acc);
			sys->register_agent_controller(0, std::move(controller));

			return std::move(sys);
		}


		std::string elevator_system_defn::floor_name(floor_t flr)
		{
			switch(flr)
			{
				case 0:
				return "Ground Floor";

				default:
				return std::string("Floor ") + std::to_string(flr);
			}
		}

		// TODO: Temp solution used for agent sensors and general state values
		std::vector< std::string > elevator_system_defn::generate_state_value_names(floor_count_t num_floors)
		{
			std::vector< std::string > names;
			names.emplace_back("Time");
			names.emplace_back("Location");
			names.emplace_back("Direction");
			names.emplace_back("Occupancy");
			names.emplace_back("# Waiting");
			names.emplace_back("# Got Off");
			names.emplace_back("# Got On");

			names.emplace_back("Total Arrivals");
			names.emplace_back("Total Boarded");
			names.emplace_back("Total Delivered");
			names.emplace_back("Distance Covered");
			names.emplace_back("Total Queuing Time");
			names.emplace_back("Avg Queuing Time");
			names.emplace_back("Max Queuing Time");
			names.emplace_back("Total Inclusive Queuing Time");
			names.emplace_back("Total Delivery Time");
			names.emplace_back("Avg Delivery Time");
			names.emplace_back("Max Delivery Time");
			names.emplace_back("Total Inclusive Delivery Time");
			names.emplace_back("Total Idle Time");

			for(floor_t f = 0; f < num_floors; ++f)
			{
				auto fname = floor_name(f);

				names.emplace_back(fname + ": # Waiting");
				names.emplace_back(fname + ": Up Pressed");
				names.emplace_back(fname + ": Up Pressed Duration");
				names.emplace_back(fname + ": # Waiting Up");
				names.emplace_back(fname + ": Down Pressed");
				names.emplace_back(fname + ": Down Pressed Duration");
				names.emplace_back(fname + ": # Waiting Down");
				names.emplace_back(fname + ": Is Destination");
			}

			return names;
		}

		std::map< std::string, int > elevator_system_defn::generate_state_value_idmap(floor_count_t num_floors)
		{
			std::map< std::string, int > mp;
			auto names = generate_state_value_names(num_floors);
			for(int id = 0; id < names.size(); ++id)
			{
				mp[names[id]] = id;
			}
			return mp;
		}

	}
}



