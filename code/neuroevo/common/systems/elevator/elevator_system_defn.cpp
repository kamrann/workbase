// elevator_system_defn.cpp

#include "elevator_system_defn.h"
#include "elevator_systemstatevalues.h"
#include "elevator_system.h"
#include "elevator_agent_defn.h"	// for create_default()

#include "system_sim/system_update_info.h"
#include "system_sim/controller.h"
#include "system_sim/passive_controller.h"	// for create_default()

#include "params/param_accessor.h"
#include "params/param_yaml.h"
#include "params/schema_builder.h"


namespace sys {
	namespace elev {

		const bimap< elevator_system_defn::CoreStateValue, std::string > elevator_system_defn::s_core_state_values = {
				{ CoreStateValue::Time, "Time" },
				{ CoreStateValue::Location, "Location" },
				{ CoreStateValue::Direction, "Direction" },
				{ CoreStateValue::Occupancy, "Occupancy" },
				{ CoreStateValue::NumWaiting, "# Waiting" },
				{ CoreStateValue::NumGotOff, "# Got Off" },
				{ CoreStateValue::NumGotOn, "# Got On" },

				{ CoreStateValue::TotalArrivals, "Total Arrivals" },
				{ CoreStateValue::TotalBoarded, "Total Boarded" },
				{ CoreStateValue::TotalDelivered, "Total Delivered" },
				{ CoreStateValue::DistanceCovered, "Distance Covered" },
				{ CoreStateValue::TotalQueuingTime, "Total Queuing Time" },
				{ CoreStateValue::AvgQueuingTime, "Avg Queuing Time" },
				{ CoreStateValue::MaxQueuingTime, "Max Queuing Time" },
				{ CoreStateValue::TotalInclusiveQueuingTime, "Total Inclusive Queuing Time" },
				{ CoreStateValue::TotalDeliveryTime, "Total Delivery Time" },
				{ CoreStateValue::AvgDeliveryTime, "Avg Delivery Time" },
				{ CoreStateValue::MaxDeliveryTime, "Max Delivery Time" },
				{ CoreStateValue::TotalInclusiveDeliveryTime, "Total Inclusive Delivery Time" },
				{ CoreStateValue::TotalIdleTime, "Total Idle Time" },
		};

		const bimap< elevator_system_defn::FloorStateValue, std::string > elevator_system_defn::s_floor_state_values = {
				{ FloorStateValue::NumWaiting, "# Waiting" },
				{ FloorStateValue::UpPressed, "Up Pressed" },
				{ FloorStateValue::UpPressedDuration, "Up Pressed Duration" },
				{ FloorStateValue::NumWaitingUp, "# Waiting Up" },
				{ FloorStateValue::DownPressed, "Down Pressed" },
				{ FloorStateValue::DownPressedDuration, "Down Pressed Duration" },
				{ FloorStateValue::NumWaitingDown, "# Waiting Down" },
				{ FloorStateValue::IsDestination, "Is Destination" },
		};


		std::unique_ptr< elevator_system_defn > elevator_system_defn::create_default()
		{
			auto defn = std::make_unique< elevator_system_defn >();

			defn->add_agent_defn("default", std::make_unique< elevator_agent_defn >());
			defn->add_controller_defn("preset", std::make_unique< passive_controller_defn >());
			//		defn->add_controller_defn("preset", std::make_unique< sys::elev::dumb_elevator_controller_defn >());

			return defn;
		}

	
		std::string elevator_system_defn::get_name() const
		{
			return "elevator";
		}

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

		std::string elevator_system_defn::update_schema_provider_for_system_core(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

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

//			auto controller_rel = update_schema_provider_for_agent_controller(provider, path);

			(*provider)[path] = [=](prm::param_accessor param_vals)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string("floor_count"))(param_vals));
				sb::append(s, provider->at(path + std::string("arrival_rate"))(param_vals));

//				sb::append(s, provider->at(path + controller_rel)(param_vals));

				sb::border(s, std::string{ "Elevator" });
				sb::enable_import(s, "system.elevator");
				return s;
			};

			return path.leaf().name();
		}

		state_value_id_list elevator_system_defn::get_system_core_state_values(prm::param_accessor acc) const
		{
			state_value_id_list sv_names;
		
			for(auto const& sv : s_core_state_values.left)
			{
				sv_names.push_back(state_value_id{ sv.second });
			}

			auto num_floors = prm::extract_as< floor_count_t >(acc["floor_count"]);
			for(size_t f = 0; f < num_floors; ++f)
			{
				state_value_id base = floor_name(f);

				for(auto const& sv : s_core_state_values.left)
				{
					sv_names.push_back(base + sv.second);
				}
			}

			return sv_names;
			
			//			return generate_state_value_names(num_floors);
		}

		system_ptr elevator_system_defn::create_system_core(prm::param_accessor acc) const
		{
			// Store param tree within defn, or externally? ex
			// Have separate system factory, or incorporate into defn? inc

			auto sys = std::make_unique< elevator_system >(acc);

			//auto controller = create_controller(acc);
			//sys->register_agent_controller(0, std::move(controller));

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
#if 0
		// TODO: Temp solution used for agent sensors and general state values
		state_value_id_list elevator_system_defn::generate_state_value_names(floor_count_t num_floors)
		{
			state_value_id_list names;
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
				state_value_id base = floor_name(f);

				names.emplace_back(base + "# Waiting");
				names.emplace_back(base + "Up Pressed");
				names.emplace_back(base + "Up Pressed Duration");
				names.emplace_back(base + "# Waiting Up");
				names.emplace_back(base + "Down Pressed");
				names.emplace_back(base + "Down Pressed Duration");
				names.emplace_back(base + "# Waiting Down");
				names.emplace_back(base + "Is Destination");
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
#endif
	}
}



