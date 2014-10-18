// system_sim.cpp : Defines the exported functions for the DLL application.
//

#include "system_sim.h"

#include "systems/elevator/elevator_system_defn.h"
#include "systems/elevator/elevator_system.h"
#include "systems/elevator/elevator_agent_defn.h"
#include "systems/elevator/dumb_elevator_controller.h"

#include "systems/physics2d/phys2d_system_defn.h"
#include "systems/physics2d/phys2d_system.h"
#include "systems/physics2d/basic_biped_defn.h"

#include "system_sim/passive_controller.h"
#include "system_sim/system_state_values.h"

#include "params/param_fwd.h"
#include "params/param_tree.h"
#include "params/param_accessor.h"

#include <yaml-cpp/yaml.h>

#include <string>
#include <memory>


extern "C" {

	char const* pw_ex_get_name()
	{
		return "syssim";
	}

	std::map< std::string, std::unique_ptr< sys::i_system_defn > > sys_defns;

	bool initialize_system_definitions()
	{
		sys_defns.clear();
		std::unique_ptr< sys::i_system_defn > defn;

		defn = sys::elev::elevator_system_defn::create_default();
		sys_defns[defn->get_name()] = std::move(defn);

		defn = sys::phys2d::phys2d_system_defn::create_default();
		sys_defns[defn->get_name()] = std::move(defn);

		return true;
	}

	char* pw_ex_run(char const* spec, void* context, pwork::pw_ex_update_cb_fn update_fn, pwork::pw_ex_buffer_alloc_fn alloc)
	{
		auto yaml_spec = YAML::Load(spec);

		auto pt = prm::param_tree::generate_from_yaml(yaml_spec);
		auto acc = prm::param_accessor{ &pt };

		// TODO: just once at dll startup
		initialize_system_definitions();

		auto sys_type = prm::extract_as< prm::enum_param_val >(acc["sys_type"])[0];
		auto defn_it = sys_defns.find(sys_type);
		if(defn_it == std::end(sys_defns))
		{
			return nullptr;
		}
		auto const& defn = defn_it->second;

		auto sys = defn->create_system(acc);
		// TODO: sys->set_random_seed(...);
		sys->initialize();

		bool running = true;
		while(running)
		{
			running = sys->update(nullptr);
		}

		YAML::Node results;
		auto result_value_names = prm::extract_as< prm::enum_param_val >(acc["results"]);
		for(auto const& res : result_value_names)
		{
			auto bound_id = sys->get_state_value_binding(sys::state_value_id::from_string(res));
			results[res] = sys->get_state_value(bound_id);
		}

		auto result = YAML::Dump(results);
		auto buf = alloc(result.length() + 1);
		std::strcpy(buf, result.c_str());
		return buf;
	}

}




