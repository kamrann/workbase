// rtp_phys_agent_body_spec.cpp

#include "rtp_phys_agent_body_spec.h"
#include "../../rtp_defs.h"
#include "bodies/test_body.h"
#include "bodies/test_quadruped_body.h"
#include "bodies/minimal_biped_body.h"
#include "bodies/test_biped_body.h"
#include "bodies/basic_spaceship.h"
#include "rtp_phys_controller.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"


namespace rtp {

	std::string const agent_body_spec::Names[] = {
		"Test Creature",
		"Test Quadruped",
		"Minimal Biped",
		"Test Biped",
		"Spaceship",
	};

}

namespace YAML {

	std::map< std::string, rtp::agent_body_spec::Type > const convert< rtp::agent_body_spec::Type >::mapping_ = {
		{ "Test Creature", rtp::agent_body_spec::Type::TestCreature },
		{ "Test Quadruped", rtp::agent_body_spec::Type::TestQuadruped },
		{ "Minimal Biped", rtp::agent_body_spec::Type::MinimalBiped },
		{ "Test Biped", rtp::agent_body_spec::Type::TestBiped },
		{ "Spaceship", rtp::agent_body_spec::Type::Spaceship },
	};
}

namespace rtp {

	void agent_body_spec::create_base_instance(prm::param_accessor param, agent_body_spec* s)
	{
		s->m_initial_pos_x = prm::extract_fixed_or_random(param["init_x"]);
		s->m_initial_pos_y = prm::extract_fixed_or_random(param["init_y"]);
		s->m_initial_orientation = prm::extract_fixed_or_random(param["init_orientation"]);
		s->m_initial_linear_dir = prm::extract_fixed_or_random(param["init_linear_angle"]);
		s->m_initial_linear_speed = prm::extract_fixed_or_random(param["init_linear_speed"]);
		s->m_initial_ang_vel = prm::extract_fixed_or_random(param["init_ang_vel"]);
	}

	void agent_body_spec::create_base_instance(prm::param& param, agent_body_spec* s)
	{
		s->m_initial_pos_x = prm::extract_fixed_or_random(param["init_x"]);
		s->m_initial_pos_y = prm::extract_fixed_or_random(param["init_y"]);
		s->m_initial_orientation = prm::extract_fixed_or_random(param["init_orientation"]);
		s->m_initial_linear_dir = prm::extract_fixed_or_random(param["init_linear_angle"]);
		s->m_initial_linear_speed = prm::extract_fixed_or_random(param["init_linear_speed"]);
		s->m_initial_ang_vel = prm::extract_fixed_or_random(param["init_ang_vel"]);
	}

	namespace sb = prm::schema;


	std::string agent_body_spec::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "spec" };
		auto path = prefix + relative;

		(*provider)[path + std::string("name")] = [](prm::param_accessor param_vals)
		{
/*			auto default_name = std::string("Temp");
			auto path = param_vals.get_current_path();
			if(path.size() > 0)
			{
				auto node = param_vals["spec_type"];
				auto spec_type = node[0].as< Type >();
				default_name = Names[spec_type];

				default_name += "[";
				auto agent_it = path.find_anywhere("agent_list");
				default_name += std::to_string(agent_it->index());
				default_name += "]";

				default_name += "[";
				auto inst_it = path.find_anywhere("instance_list");
				default_name += std::to_string(inst_it->index());
				default_name += "]";
			}
*/
			auto s = sb::string("name");// , default_name);
			sb::label(s, "Name");
			sb::trigger(s, "agentspec_name_modified");
			return s;
		};

		(*provider)[path + std::string("spec_type")] = [](prm::param_accessor)
		{
			auto s = sb::enum_selection(
				"spec_type",
				std::vector< std::string >{ begin(Names), end(Names) },
				0, 1
			);
			sb::label(s, "Spec Type");
			sb::trigger(s, "spec_type");
			return s;
		};


		path += std::string("body_spec");

		auto testbody_rel = test_body::spec::update_schema_providor(provider, path);
		auto minimal_biped_rel = minimal_biped_body::spec::update_schema_providor(provider, path);
		auto biped_rel = test_biped_body::spec::update_schema_providor(provider, path);
		auto quadruped_rel = test_quadruped_body::spec::update_schema_providor(provider, path);
		auto spaceship_rel = basic_spaceship::spec::update_schema_providor(provider, path);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("body_spec");
			auto type_node = param_vals["spec_type"];
			auto enum_sel = (!type_node || type_node.as< prm::is_unspecified >()) ? Type::Default : type_node[0].as< Type >();
			switch(enum_sel)
			{
				case Type::TestCreature:
				sb::append(s, provider->at(path + testbody_rel)(param_vals));
				break;
				case Type::MinimalBiped:
				sb::append(s, provider->at(path + minimal_biped_rel)(param_vals));
				break;
				case Type::TestBiped:
				sb::append(s, provider->at(path + biped_rel)(param_vals));
				break;
				case Type::TestQuadruped:
				sb::append(s, provider->at(path + quadruped_rel)(param_vals));
				break;
				case Type::Spaceship:
				sb::append(s, provider->at(path + spaceship_rel)(param_vals));
				break;
			}
			sb::unborder(s);
			sb::update_on(s, "spec_type");
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("name"))(param_vals));
			sb::append(s, provider->at(path + std::string("spec_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("body_spec"))(param_vals));
			sb::unborder(s);
			sb::enable_import(s, "physics2d.agent_spec");
			return s;
		};

		return relative;
	}

	agent_body_spec* agent_body_spec::create_instance(prm::param_accessor param)
	{
		agent_body_spec* result = nullptr;
		param.push_relative_path(prm::qualified_path("spec"));
		auto type_node = param["spec_type"];
		if(type_node.as< prm::is_unspecified >())
		{
			throw prm::required_unspecified("Agent Specification Type");
		}

		auto type = type_node[0].as< Type >();
		switch(type)
		{
			case TestCreature:
			result = test_body::spec::create_instance(param);
			break;

			case TestQuadruped:
			result = test_quadruped_body::spec::create_instance(param);
			break;

			case MinimalBiped:
			result = minimal_biped_body::spec::create_instance(param);
			break;

			case TestBiped:
			result = test_biped_body::spec::create_instance(param);
			break;

			case Spaceship:
			result = basic_spaceship::spec::create_instance(param);
			break;
		}

		param.pop_relative_path();
		return result;
	}

	agent_sensor_name_list agent_body_spec::sensor_inputs(Type type)
	{
		switch(type)
		{
			case TestCreature:
			return test_body::spec::sensor_inputs();
			case TestQuadruped:
			return test_quadruped_body::spec::sensor_inputs();
			case MinimalBiped:
			return minimal_biped_body::spec::sensor_inputs();
			case TestBiped:
			return test_biped_body::spec::sensor_inputs();
			case Spaceship:
			return basic_spaceship::spec::sensor_inputs();

			default:
			assert(false);
			return{};
		}
	}

	size_t agent_body_spec::num_effectors(Type type)
	{
		switch(type)
		{
			case TestCreature:
			return test_body::spec::num_effectors();
			case TestQuadruped:
			return test_quadruped_body::spec::num_effectors();
			case MinimalBiped:
			return minimal_biped_body::spec::num_effectors();
			case TestBiped:
			return test_biped_body::spec::num_effectors();
			case Spaceship:
			return basic_spaceship::spec::num_effectors();

			default:
			assert(false);
			return{};
		}
	}

	agent_body_spec::agent_body_spec(Type t): m_type(t)
	{}
}



