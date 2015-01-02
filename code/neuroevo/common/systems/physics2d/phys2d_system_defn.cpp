// phys2d_system_defn.cpp

#include "phys2d_system_defn.h"
#include "phys2d_systemstatevalues.h"
#include "phys2d_system.h"
/*
#include "basic_biped_defn.h"		// for create_default()
#include "snakebot_defn.h"		// for create_default()
#include "pole_cart_defn.h"		// for create_default()
#include "skier_defn.h"		// for create_default()
#include "magbot_defn.h"		// for create_default()
*/
#include "generic_agent_defn.h"		// for create_default()

#include "ground_scenario_defn.h"	// for create_default()
#include "ski_jump_scenario_defn.h"	// for create_default()
#include "aerials_scenario_defn.h"	// for create_default()

#include "system_sim/system_update_info.h"
#include "system_sim/controller.h"
#include "system_sim/passive_controller.h"	// for create_default()
#include "system_sim/generic_ui_controller.h"	// for create_default()
#include "system_sim/direct_bindings_ui_controller.h"	// for create_default()
#include "test_snakebot_controller.h"	// for create_default()
#include "test_skier_controller.h"	// for create_default()


namespace sys {
	namespace phys2d {

		const bimap< phys2d_system_defn::StateValue, std::string > phys2d_system_defn::s_state_values = {
				{ StateValue::Time, "Time" },
		};


		std::unique_ptr< phys2d_system_defn > phys2d_system_defn::create_default()
		{
			auto defn = std::make_unique< phys2d_system_defn >();

			defn->add_scenario_defn(std::make_unique< ground_scenario_defn >());
			defn->add_scenario_defn(std::make_unique< ski_jump_scenario_defn >());
			defn->add_scenario_defn(std::make_unique< aerials_scenario_defn >());

			defn->add_agent_defn("default", std::make_unique< generic_agent_defn >());
/*			defn->add_agent_defn("default", std::make_unique< basic_biped_defn >());
			defn->add_agent_defn("default", std::make_unique< snakebot_defn >());
			defn->add_agent_defn("default", std::make_unique< skier_defn >());
			defn->add_agent_defn("default", std::make_unique< pole_cart_defn >());
			defn->add_agent_defn("default", std::make_unique< magbot_defn >());
*/
			defn->add_controller_defn("preset", std::make_unique< passive_controller_defn >());
//			defn->add_controller_defn("preset", std::make_unique< test_snakebot_controller_defn >());
//			defn->add_controller_defn("preset", std::make_unique< test_skier_controller_defn >());

			// todo: hacky
			auto defn_raw = defn.get();
			defn->add_controller_defn("interactive", std::make_unique< generic_ui_controller_defn >(
				[defn_raw]() -> ddl::dep_function < size_t >
			{
				return defn_raw->get_inst_num_effectors_fn();
			}));
			defn->add_controller_defn("interactive", std::make_unique< direct_bindings_ui_controller_defn >(
				[defn_raw]() -> ddl::dep_function < size_t >
			{
				return defn_raw->get_inst_num_effectors_fn();
			}));

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

		ddl::defn_node phys2d_system_defn::get_system_core_defn(ddl::specifier& spc)
		{
			auto get_scenarios_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this](ddl::navigator nav)
			{
				auto scenario_names = ddl::enum_defn_node::enum_str_set_t{};
				for(auto const& scen : m_scenario_defns)
				{
					scenario_names.insert(std::end(scenario_names), scen.second->get_name());
				}
				return scenario_names;
			}
			};

			ddl::defn_node scenario_type = spc.enumeration("scenario_type")
				(ddl::spc_range < size_t > { 1, 1 })
				(ddl::define_enum_func{ get_scenarios_fn })
				;

			auto cond_scen_spc = spc.conditional("scenario_?");
			for(auto const& entry : m_scenario_defns)
			{
				ddl::defn_node dn = entry.second->get_defn(spc);
				cond_scen_spc = cond_scen_spc
					(ddl::spc_condition{ ddl::cnd::equal{ ddl::node_ref{ scenario_type }, entry.first }, dn })
					;
			}

			ddl::defn_node cond_scen = cond_scen_spc;

			ddl::defn_node duration = spc.realnum("duration")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 10.0 })
				;

			auto rf_scenario_type = ddl::node_ref{ scenario_type };
			core_state_values_fn_.set_fn([this, rf_scenario_type](ddl::navigator nav) -> state_value_id_list
			{
				state_value_id_list sv_names;
				for(auto const& sv : s_state_values.left)
				{
					sv_names.push_back(state_value_id{ sv.second });
				}

				auto scenario_type_nav = ddl::nav_to_ref(rf_scenario_type, nav);
				auto scenario_name_enum = scenario_type_nav.get().as_enum_str();
				if(!scenario_name_enum.empty())
				{
					auto scenario_name = *scenario_name_enum.begin();
					// TODO: dependencies of this call
					auto scenario_svs = m_scenario_defns.at(scenario_name)->get_state_values({});// TODO: nav["scenario"]);
					//
					sv_names.insert(std::end(sv_names), std::begin(scenario_svs), std::end(scenario_svs));
				}
				return sv_names;
			});
			core_state_values_fn_.add_dependency(ddl::node_dependency(rf_scenario_type));

			return spc.composite("phys2d_sys_core")(ddl::define_children{}
				("updates_per_sec", spc.integer("updates_per_sec")(ddl::spc_default < ddl::integer_defn_node::value_t > { 60 })(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 }))
				("velocity_iterations", spc.integer("velocity_iterations")(ddl::spc_default < ddl::integer_defn_node::value_t > { 4 })(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 }))
				("position_iterations", spc.integer("position_iterations")(ddl::spc_default < ddl::integer_defn_node::value_t > { 2 })(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 }))
				("scenario_type", scenario_type)
				("scenario", cond_scen)
				("duration", duration)
				);
		}


		ddl::dep_function< state_value_id_list > phys2d_system_defn::get_system_core_state_values_fn() const//ddl::navigator nav) const
		{
			return core_state_values_fn_;
		}

		system_ptr phys2d_system_defn::create_system_core(ddl::navigator nav) const
		{
			auto update_rate = nav["updates_per_sec"].get().as_int();
			auto vel_its = nav["velocity_iterations"].get().as_int();
			auto pos_its = nav["position_iterations"].get().as_int();

			auto scenario_name = nav["scenario_type"].get().as_single_enum_str();
			auto scen = m_scenario_defns.at(scenario_name)->create_scenario(nav["scenario"][(size_t)0]);

			auto duration = nav["duration"].get().as_real();

			auto sys = std::make_unique< phys2d_system >(std::move(scen), duration, update_rate, vel_its, pos_its);
			return std::move(sys);
		}

	}
}



