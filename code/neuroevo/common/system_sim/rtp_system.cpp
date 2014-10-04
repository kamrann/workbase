// rtp_system.cpp

#include "rtp_system.h"
//#include "rtp_system_agents_data.h"

#include "rtp_controller.h"
/*
#include "phys/rtp_phys_system.h"
#include "radial_detection/rtp_radial_detection_system.h"
#include "elevator/rtp_elevator_system.h"
*/
#include "params/schema_builder.h"
#include "params/param_accessor.h"
#include "params/param_yaml.h"

#include <iomanip>

#if 0
namespace YAML {
	std::map< std::string, sys::SystemType > const convert< sys::SystemType >::mapping_ = {
			{ "Physics 2D", sys::SystemType::Physics2D },
			{ "Detection 2D", sys::SystemType::Detection2D },
			{ "Elevator", sys::SystemType::Elevator },
	};
}
#endif

namespace sys {
#if 0
	const std::array< std::string, SystemType::Count > SystemNames = {
		//	"Noughts & Crosses",
		//	"Ship & Thrusters 2D",
		"Physics 2D",
		"Detection 2D",
		"Elevator",
	};

	namespace sb = prm::schema;

	std::string i_system::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable)
	{
		auto relative = std::string{ "sys_params" };
		auto path = prefix + relative;

		(*provider)[path + std::string("sys_type")] = [](prm::param_accessor)
		{
			auto s = sb::enum_selection(
				"sys_type",
				std::vector < std::string > { begin(SystemNames), end(SystemNames) },
				0, 1);
			sb::label(s, "System Type");
			sb::trigger(s, "sys_type_changed");
			return s;
		};

		path += std::string("type_specific_sys_params");

		auto phys2d_rel = sys::phys_system::update_schema_providor(provider, path, evolvable);
		auto detection2d_rel = sys::rd_system::update_schema_providor(provider, path, evolvable);
		auto elevator_rel = sys::elevator_system::update_schema_providor(provider, path, evolvable);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("type_specific_sys_params");
			auto node = param_vals["sys_type"];
			auto enum_sel = (!node || node.as< prm::is_unspecified >()) ? SystemType::Default : node[0].as< SystemType >();
			switch(enum_sel)
			{
				case SystemType::Physics2D:
				sb::append(s, provider->at(path + phys2d_rel)(param_vals));
				break;
				case SystemType::Detection2D:
				sb::append(s, provider->at(path + detection2d_rel)(param_vals));
				break;
				case SystemType::Elevator:
				sb::append(s, provider->at(path + elevator_rel)(param_vals));
				break;
			}
			sb::unborder(s);
			sb::update_on(s, "sys_type_changed");
			return s;
		};

		path.pop();

		(*provider)[path + std::string("random_seed")] = [](prm::param_accessor)
		{
			auto s = sb::integer(
				"random_seed",
				0,
				0);
			sb::label(s, "Random Seed");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("sys_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("type_specific_sys_params"))(param_vals));
			if(!evolvable)
			{
				sb::append(s, provider->at(path + std::string("random_seed"))(param_vals));
			}
			sb::label(s, "System");
			return s;
		};

		return relative;
	}

	std::unique_ptr< i_system_factory > i_system::generate_system_factory(prm::param_accessor param_vals)
	{
		std::unique_ptr< i_system_factory > result{};

		param_vals.push_relative_path(prm::qualified_path("sys_params"));
		auto sys_type_node = param_vals["sys_type"];
		if(sys_type_node.as< prm::is_unspecified >())
		{
			throw prm::required_unspecified("System Type");
		}
		SystemType type = sys_type_node[0].as< SystemType >();
		param_vals.push_relative_path(prm::qualified_path("type_specific_sys_params"));

		switch(type)
		{
			/*		case NoughtsAndCrosses:
			return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* >(nullptr, nullptr, nullptr, nullptr);

			case ShipAndThrusters2D:
			return rtp_sat::sat_system< WorldDimensionality::dim2D >::create_instance(param_list[1], evolvable);
			*/
			case SystemType::Physics2D:
			result = sys::phys_system::generate_factory(param_vals);
			break;
			case SystemType::Detection2D:
			result = sys::rd_system::generate_factory(param_vals);
			break;
			case SystemType::Elevator:
			result = sys::elevator_system::generate_factory(param_vals);
			break;
		}

		param_vals.pop_relative_path();

		if(param_vals["random_seed"])
		{
			auto rseed = param_vals["random_seed"].as< uint32_t >();
			// TODO: 0 should be a valid seed, we need control allowing integer input and also special options, in this
			// case <auto>.
			if(rseed != 0)
			{
				result->set_random_seed(rseed);
			}
		}

		param_vals.pop_relative_path();

		return result;
	}

	agents_data i_system::generate_agents_data(prm::param_accessor param_vals, bool evolvable)
	{
		agents_data result{};

		param_vals.push_relative_path(prm::qualified_path("sys_params"));
		SystemType type = param_vals["sys_type"][0].as< SystemType >();
		param_vals.push_relative_path(prm::qualified_path("type_specific_sys_params"));

		switch(type)
		{
			/*		case NoughtsAndCrosses:
			return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* >(nullptr, nullptr, nullptr, nullptr);

			case ShipAndThrusters2D:
			return rtp_sat::sat_system< WorldDimensionality::dim2D >::create_instance(param_list[1], evolvable);
			*/
			case SystemType::Physics2D:
			result = sys::phys_system::generate_agents_data(param_vals, evolvable);
			break;
			case SystemType::Detection2D:
			result = sys::rd_system::generate_agents_data(param_vals, evolvable);
			break;
			case SystemType::Elevator:
			result = sys::elevator_system::generate_agents_data(param_vals, evolvable);
			break;
		}

		param_vals.pop_relative_path();
		param_vals.pop_relative_path();

		return result;
	}

	std::vector< std::string > i_system::get_agent_type_names(SystemType sys_type)
	{
		switch(sys_type)
		{
			case SystemType::Physics2D:
			return phys_system::get_agent_type_names();
			case SystemType::Detection2D:
			return rd_system::get_agent_type_names();
			case SystemType::Elevator:
			return elevator_system::get_agent_type_names();

			default:
			throw std::exception("Invalid system type");
		}
	}

	std::vector< std::string > i_system::get_agent_sensor_names(SystemType sys_type, prm::param agent_type, prm::param_accessor param_vals)
	{
		switch(sys_type)
		{
			case SystemType::Physics2D:
			return phys_system::get_agent_sensor_names(agent_type, param_vals);
			case SystemType::Detection2D:
			return rd_system::get_agent_sensor_names(agent_type, param_vals);
			case SystemType::Elevator:
			return elevator_system::get_agent_sensor_names(agent_type, param_vals);

			default:
			throw std::exception("Invalid system type");
		}
	}

	size_t i_system::get_agent_num_effectors(SystemType sys_type, prm::param agent_type)
	{
		switch(sys_type)
		{
			case SystemType::Physics2D:
			return phys_system::get_agent_num_effectors(agent_type);
			case SystemType::Detection2D:
			return rd_system::get_agent_num_effectors(agent_type);
			case SystemType::Elevator:
			return elevator_system::get_agent_num_effectors(agent_type);

			default:
			throw std::exception("Invalid system type");
		}
	}

	std::vector< std::string > i_system::get_system_state_values(prm::param_accessor param_vals)
	{
		auto sys_type = param_vals["sys_type"][0].as< SystemType >();
		switch(sys_type)
		{
			case SystemType::Physics2D:
			return phys_system::get_state_values(param_vals);
			case SystemType::Detection2D:
			return rd_system::get_state_values(param_vals);
			case SystemType::Elevator:
			return elevator_system::get_state_values(param_vals);

			default:
			throw std::exception("Invalid System Type");
		}
	}
#endif

	void i_system::set_random_seed(unsigned long rseed)
	{
		m_rgen.seed(rseed);
	}

	void i_system::output_performance_data(perf_data_t const& pd, std::ostream& strm, size_t samples)
	{
		typedef std::chrono::duration< double > d_dur;
		auto total_time = d_dur{};
		for(auto const& entry : pd)
		{
			total_time += entry.second;
		}
		
		strm << "\nOverall performance data:\n";
		auto denom = 0.01 * total_time.count();
		strm << std::setprecision(2);
		for(auto const& entry : pd)
		{
			strm << entry.first << ": " << entry.second.count() / denom << "%" <<
				" [" << (entry.second.count() / samples) << "s/trial]" << std::endl;
		}
		strm << std::endl;
	}


#if 0
	std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > i_system::create_instance(prm::param_accessor param_vals, bool evolvable)
	{
		std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > result{ nullptr, nullptr, nullptr, nullptr, nullptr };
		param_vals.push_relative_path(prm::qualified_path("sys_params"));
		SystemType type = param_vals["sys_type"][0].as< SystemType >();
		param_vals.push_relative_path(prm::qualified_path("type_specific_sys_params"));
		switch(type)
		{
			/*		case NoughtsAndCrosses:
			return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* >(nullptr, nullptr, nullptr, nullptr);

			case ShipAndThrusters2D:
			return rtp_sat::sat_system< WorldDimensionality::dim2D >::create_instance(param_list[1], evolvable);
			*/
			case Physics2D:
			{
				result = sys::phys_system::create_instance(param_vals, evolvable);
				break;
			}
		}

		param_vals.pop_relative_path();
		param_vals.pop_relative_path();

		return result;
	}

	std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > i_system::create_instance(prm::param& param_vals, bool evolvable)
	{
		SystemType type = param_vals["type"].as< SystemType >();
		switch(type)
		{
			/*		case NoughtsAndCrosses:
			return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* >(nullptr, nullptr, nullptr, nullptr);

			case ShipAndThrusters2D:
			return rtp_sat::sat_system< WorldDimensionality::dim2D >::create_instance(param_list[1], evolvable);
			*/
			case Physics2D:
			{
				return sys::phys_system::create_instance(param_vals, evolvable);
			}

			default:
			return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* >(nullptr, nullptr, nullptr, nullptr, nullptr);
		}
	}
#endif

}
