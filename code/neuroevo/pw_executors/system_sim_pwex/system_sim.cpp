// system_sim.cpp : Defines the exported functions for the DLL application.
//

#include "system_sim.h"

#include "systems/elevator/elevator_system_defn.h"
#include "systems/elevator/elevator_system.h"
#include "systems/elevator/dumb_elevator_controller.h"
//#include "system_sim/"

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

	char* pw_ex_run(char const* spec, void* context, pwork::pw_ex_update_cb_fn update_fn, pwork::pw_ex_buffer_alloc_fn alloc)
	{
		auto yaml_spec = YAML::Load(spec);

		auto pt = prm::param_tree::generate_from_yaml(yaml_spec);
		auto acc = prm::param_accessor{ &pt };

		// here determine system type
		auto defn = std::make_shared< sys::elev::elevator_system_defn >();
		defn->add_controller_defn("Preset", std::make_unique< sys::elev::dumb_elevator_controller_defn >());

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
			results[res] = sys->get_state_value(res);
		}

		auto result = YAML::Dump(results);
		auto buf = alloc(result.length() + 1);
		std::strcpy(buf, result.c_str());
		return buf;
	}

}




