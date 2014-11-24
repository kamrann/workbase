// evolved_system_domain_defn.cpp

#include "evolved_system_domain_defn.h"

#include "system_sim/system.h"

#include "systems/elevator/elevator_system_defn.h"
#include "systems/physics2d/phys2d_system_defn.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace sys {
	namespace ev {

		namespace sb = prm::schema;

		void evolved_system_domain_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			system_defn.update_schema_provider(provider, path + std::string{ "sys" });
			ev_con_defn.update_schema_provider(provider, path + std::string{ "ev_con" });
			obj_defn.update_schema_provider(provider, path + std::string{ "objective" });

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string{ "sys" })(acc));
				sb::append(s, provider->at(path + std::string{ "ev_con" })(acc));
				sb::append(s, provider->at(path + std::string{ "objective" })(acc));
				return s;
			};
		}

		std::unique_ptr< evolved_system_domain > evolved_system_domain_defn::generate(prm::param_accessor acc)
		{
			auto sys = system_defn.generate(acc("sys"));
			auto mp = ev_con_defn.generate(acc("ev_con"));
			auto sys_ptr = sys.get();
			auto obj = obj_defn.generate(acc("objective"), [sys_ptr](state_value_id const& svid)
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
				[sp_sys](prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
			{
				sp_sys->update_schema_provider(provider, prefix);
			},
				[sp_sys](prm::param_accessor acc)
			{
				return sp_sys->create_system(acc);
			}
			);

			sys_defn_mp[sp_sys->get_name()] = sp_sys;
		}

		i_system_defn const* evolved_system_domain_defn::current_system_defn(prm::param_accessor acc)
		{
			if(acc.is_available("sys"))
			{
				acc.unlock();	// TODO: major hack.
					// may need to distinguish between move locking to descendents (because we know there aren't
					// any) and still allowing movement to other parts up the tree. basically, it's a mess.

				acc = acc("sys")(prm::param_accessor::LookupMode::Child);
				if(!prm::is_unspecified(acc["type"]))
				{
					auto sys_name = prm::extract_as< prm::enum_param_val >(acc["type"])[0];
					return sys_defn_mp.at(sys_name).get();
				}
			}
			return nullptr;
		}

		evolved_system_domain_defn::evolved_system_domain_defn():
			ev_con_defn([this](prm::param_accessor acc)
		{
			return current_system_defn(acc);
		}),
			obj_defn([this](prm::param_accessor acc) -> state_value_id_list
		{
			auto sysdefn = current_system_defn(acc);
			if(sysdefn)
			{
				return sysdefn->get_system_state_values(acc);
			}
			else
			{
				return{};
			}
		})
		{
			register_system_type(sys::elev::elevator_system_defn::create_default());
			register_system_type(sys::phys2d::phys2d_system_defn::create_default());
		}

	}
}




