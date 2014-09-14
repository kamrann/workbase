// rtp_phys_agent_body.cpp

#include "rtp_phys_agent_body.h"
#include "rtp_phys_agent_body_spec.h"
#include "rtp_phys_controller.h"
#include "rtp_phys_system.h"	// for agent_phys_specification only...

#include "bodies/test_body.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"


namespace rtp {

	std::vector< std::pair< prm::qualified_path, std::string > > get_available_agent_specs(prm::param_accessor param_vals)
	{
		std::vector< std::pair< prm::qualified_path, std::string > > names;

		auto node = param_vals["agent_spec_list"];
		auto agent_spec_list_path = resolve_id("agent_spec_list", param_vals.get_root(), param_vals.get_current_path(), param_vals.get_all_paths());
		for(auto agent_entry : node)
		{
			auto agent_repeat_num = agent_entry.first.as< unsigned int >();
			auto agent = agent_entry.second;

			auto rel_path = prm::qualified_path({ "spec" }) + std::string{ "name" };
			auto name_node = prm::find_value(agent, rel_path);
			assert(name_node);

			auto spec_path = agent_spec_list_path;
			spec_path.leaf().set_index(agent_repeat_num);
			spec_path += std::string("spec");

			names.emplace_back(std::make_pair(
				spec_path,
				name_node.as< std::string >()
				));
		}

		return names;
	}

	namespace sb = prm::schema;

	std::string agent_body::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable)
	{
		auto relative = std::string{ "agent" };
		auto path = prefix + relative;

		(*provider)[path + std::string("spec_reference")] = [=](prm::param_accessor param_vals)
		{
			// Get a list of names of 
			auto spec_names = get_available_agent_specs(param_vals);
			auto num_available_specs = spec_names.size();
			std::vector< std::pair< std::string, std::string > > spec_elements(num_available_specs);
			int idx = 0;
			std::transform(
				begin(spec_names),
				end(spec_names),
				begin(spec_elements),
				[](std::pair< prm::qualified_path, std::string > const& in)
			{
				return std::make_pair(
					in.second,// TODO: now using names to id, if keep this, need to enforce uniqueness
						// in.first.to_string(),
					in.second
					);
			});

			auto spec_ref = sb::enum_selection(
				"spec_reference",
				spec_elements,
				0, 1
				);
			sb::label(spec_ref, "Spec");
			sb::update_on(spec_ref, "$agentspec_name_modified");
			sb::update_on(spec_ref, "$agentspec_added_removed");

			return spec_ref;
		};

		// TODO: following dependent on agent type
//		agent_body_spec::Type type = get agent type;

		path += std::string{ "initial_conditions" };

		(*provider)[path + std::string("init_x")] = [](prm::param_accessor)
		{
			auto s = sb::random("init_x", 0.0, boost::none, boost::none, -10.0, 10.0);
			sb::label(s, "X Pos");
			return s;
		};
		(*provider)[path + std::string("init_y")] = [](prm::param_accessor)
		{
			auto s = sb::random("init_y", 0.0, boost::none, boost::none, -10.0, 10.0);
			sb::label(s, "Y Pos");
			return s;
		};
		(*provider)[path + std::string("init_orientation")] = [](prm::param_accessor)
		{
			auto s = sb::random("init_orientation", 0.0, -180.0, 180.0);
			sb::label(s, "Orientation");
			return s;
		};
		(*provider)[path + std::string("init_linear_angle")] = [](prm::param_accessor)
		{
			auto s = sb::random("init_linear_angle", 0.0, 0.0, 360.0);
			sb::label(s, "Linear Motion Angle");
			return s;
		};
		(*provider)[path + std::string("init_linear_speed")] = [](prm::param_accessor)
		{
			auto s = sb::random("init_linear_speed", 0.0, 0.0, boost::none, 0.0, 10.0);
			sb::label(s, "Linear Speed");
			return s;
		};
		(*provider)[path + std::string("init_ang_vel")] = [](prm::param_accessor)
		{
			auto s = sb::random("init_ang_vel", 0.0, boost::none, boost::none, -90.0, 90.0);
			sb::label(s, "Angular Velocity");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("initial_conditions");
			sb::append(s, provider->at(path + std::string("init_x"))(param_vals));
			sb::append(s, provider->at(path + std::string("init_y"))(param_vals));
			sb::append(s, provider->at(path + std::string("init_orientation"))(param_vals));
			sb::append(s, provider->at(path + std::string("init_linear_angle"))(param_vals));
			sb::append(s, provider->at(path + std::string("init_linear_speed"))(param_vals));
			sb::append(s, provider->at(path + std::string("init_ang_vel"))(param_vals));
			sb::label(s, "Initial Conditions");
			return s;
		};

		path.pop();

		auto controller_rel = i_controller::update_schema_providor(provider, path, evolvable);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("spec_reference"))(param_vals));
			sb::append(s, provider->at(path + std::string("initial_conditions"))(param_vals));
			sb::append(s, provider->at(path + controller_rel)(param_vals));
			sb::unborder(s);
//			sb::enable_import(s, "TODO:");
			return s;
		};

		return relative;
	}

	agent_body::agent_body(phys_agent_specification const& spec, phys_system* system)
	{
		// TODO: Stuff should probably be moved up to i_agent level

		// Temp hack
		m_name = spec.spec_acc["name"].as< std::string >();
		auto temp = spec.inst_acc.get_current_path();
		m_name += "[" + std::to_string(temp.leaf().index()) + "]";
	}

	agent_sensor_list agent_body::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
	{
		auto named_sensors = get_sensors();
		agent_sensor_list sensor_ids;
		for(auto const& name: named_inputs)
		{
			auto it = std::find(std::begin(named_sensors), std::end(named_sensors), name);
			assert(it != std::end(named_sensors));
			agent_sensor_id id = it - std::begin(named_sensors);
			sensor_ids.push_back(id);
		}
		std::sort(std::begin(sensor_ids), std::end(sensor_ids), std::less<>());
		return sensor_ids;
	}

	double agent_body::get_sensor_value(agent_sensor_id const& sensor) const
	{
		switch(sensor)
		{
			case Position_X:
			return get_position().x;
			case Position_Y:
			return get_position().y;
			case WorldVelocity_X:
			return get_linear_velocity().x;
			case WorldVelocity_Y:
			return get_linear_velocity().y;
			case KineticEnergy:
			return get_kinetic_energy();

			default:
			throw std::exception("Invalid Sensor Id");
		}
	}

	/*	i_agent_factory* agent_body::create_agent_factory(prm::param_accessor spec_acc, prm::param_accessor inst_acc)
	{
		auto spec_type = spec_acc["spec_type"].as< agent_body_spec::Type >();
		switch(spec_type)
		{
			case agent_body_spec::Type::TestCreature:
			return new agent_factory([&](i_system* system)
			{
				return new test_body(
					spec_acc,
					inst_acc,
					static_cast< phys_system* >(system));
			});

			default:
			return nullptr;
		}
	}
	*/
}

