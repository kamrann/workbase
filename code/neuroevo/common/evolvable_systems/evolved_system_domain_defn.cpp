// evolved_system_domain_defn.cpp

#include "evolved_system_domain_defn.h"

#include "system_sim/system.h"

//#include "systems/elevator/elevator_system_defn.h"
#include "systems/physics2d/phys2d_system_defn.h"

//#include "params/param_accessor.h"
//#include "params/schema_builder.h"


namespace sys {
	namespace ev {

		ddl::defn_node evolved_system_domain_defn::get_defn(ddl::specifier& spc)
		{
			ddl::defn_node sys = system_defn.get_defn(spc);
			auto rf_cur_sys = system_defn.get_choice_ref();
			///////////// must be initialized before calling get_defn on below objects ...
			current_system_defn_fn_ = ddl::dep_function < i_system_defn const* > { [this, rf_cur_sys](ddl::navigator nav) -> i_system_defn const*
			{
				nav = ddl::nav_to_ref(rf_cur_sys, nav);
				if(nav)
				{
					auto sys_name_enum = nav.get().as_enum_str();
					if(sys_name_enum.size() == 1)
					{
						auto sys_name = *sys_name_enum.begin();
						return sys_defn_mp.at(sys_name).get();
					}
				}
				return nullptr;
			}
			};
			current_system_defn_fn_.add_dependency(ddl::node_dependency(rf_cur_sys));

			state_vals_fn_ = ddl::dep_function < state_value_id_list > { [this](ddl::navigator nav) -> state_value_id_list
			{
				auto sys = current_system_defn_fn_(nav);
				if(sys)
				{
					// This needs dependencies on a per-system type level.
					// Ie. there are defn nodes created within the defn hierarchy of system type A which 
					// result in an altered state value list when A is selected as the current system.
					// Likewise B, C etc have their own set of defn nodes affecting this when they are current.
					// This dependency function, for it to be callable regardless of current system, needs
					// to have all of those dependencies registered with it.
					// See below.
					return sys->get_system_state_values_fn()(nav);
				}
				return{};
			}
			};
			state_vals_fn_.add_dependency(current_system_defn_fn_);
			// TODO: Re above point, currently registering all dependencies as necessary here.
			// However, really some kind of hierarchical/path based dependencies seem in order, rather than the
			// current way in which dependencies are essentially flattened to a list of all referenced defn nodes.
			// If we can specify that the above function depends on, for each system type S, a subset of defn nodes
			// of S, VIA the root defn node of S, then we should get a performance improvement since those 
			// references would not even be evaluated if the root of S could not be resolved - ie. we wouldn't
			// be wasting effort resolving references within the defn tree of system type X, if the currently
			// selected system type was Y.
			for(auto const& entry : sys_defn_mp)
			{
				state_vals_fn_.add_dependency(entry.second->get_system_state_values_fn());
			}

			ddl::defn_node ev_con = ev_con_defn.get_defn(spc);
			ddl::defn_node obj = obj_defn.get_defn(spc);

			return spc.composite("ev_sys_domain")(ddl::define_children{}
				("sys", sys)
				("ev_con", ev_con)
				("obj", obj)
				);
		}

		std::unique_ptr< evolved_system_domain > evolved_system_domain_defn::generate(ddl::navigator nav)
		{
			auto sys = system_defn.generate(nav["sys"]);
			auto mp = ev_con_defn.generate(nav["ev_con"]);
			auto sys_ptr = sys.get();
			auto obj = obj_defn.generate(nav["obj"], [sys_ptr](state_value_id const& svid)
			{
				auto bound = sys_ptr->get_state_value_binding(svid);
				return sys_ptr->get_state_value(bound);
			});
			return std::make_unique< evolved_system_domain >(
				std::move(sys),
				std::move(mp),
				std::move(obj)
				);
		}

		void evolved_system_domain_defn::register_system_type(system_defn_ptr defn)
		{
			defn->add_controller_defn("evolved_cls", std::make_unique< evolvable_controller_defn >());

			auto sp_sys = std::shared_ptr< i_system_defn >(defn.release());
			system_defn.register_option(sp_sys->get_name(),
				[sp_sys](ddl::specifier& spc)
			{
				return sp_sys->get_defn(spc);
			},
				[sp_sys](ddl::navigator nav)
			{
				return sp_sys->create_system(nav);
			}
			);

			sys_defn_mp[sp_sys->get_name()] = sp_sys;
		}

		evolved_system_domain_defn::evolved_system_domain_defn():
			ev_con_defn(current_system_defn_fn_, state_vals_fn_),
			obj_defn(state_vals_fn_)
		{
// TODO:			register_system_type(sys::elev::elevator_system_defn::create_default());
			register_system_type(sys::phys2d::phys2d_system_defn::create_default());
		}

	}
}




