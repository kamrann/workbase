// client_ddl.h

#include "system_sim_client.h"


ddl::defn_node client_ddl::get_defn(ddl::specifier& spc, std::map< std::string, std::shared_ptr< sys::i_system_defn > >& sys_defns)
{
	auto get_systems_fn = ddl::enum_defn_node::enum_values_str_fn_t{
		[this, &sys_defns](ddl::navigator nav)
		{
			ddl::enum_defn_node::enum_str_set_t systems;
			std::transform(
				std::begin(sys_defns),
				std::end(sys_defns),
				std::inserter(systems, std::end(systems)),
				[](std::map< std::string, std::shared_ptr< sys::i_system_defn > >::value_type const& entry)
			{
				return entry.first;
			});
			return systems;
		}
	};

	ddl::defn_node sys_type = spc.enumeration("sys_type")
		(ddl::spc_range< size_t >{ 1, 1 })
		(ddl::define_enum_func{ get_systems_fn })
		;

	auto sys_details_spc = spc.conditional("sys_details_?");
	for(auto const& entry : sys_defns)
	{
		auto const& sd = entry.second;
		ddl::defn_node dn = sd->get_defn(spc);
		sys_details_spc = sys_details_spc
			(ddl::spc_condition{ ddl::cnd::equal{ ddl::node_ref{ sys_type }, sd->get_name() }, dn });
	}
	ddl::defn_node sys_details = sys_details_spc;

	auto get_system_svs_fn = ddl::enum_defn_node::enum_values_str_fn_t{
		[this, &sys_defns, sys_type](ddl::navigator nav)
		{
			ddl::enum_defn_node::enum_str_set_t svs;

			nav = ddl::nav_to_ref(ddl::node_ref{ sys_type }, nav);
			if(!nav || nav.get().as_enum().empty())
			{
				return svs;
			}
			auto sel_sys = nav.get().as_single_enum_str();
			// TODO: lots more dependencies going on here.
			// Need the virtual function to not do the work, but rather return the stored dep_function,
			// and then pain.
			auto svidlist = sys_defns.at(sel_sys)->get_system_state_values_fn()(nav);

			std::transform(
				std::begin(svidlist),
				std::end(svidlist),
				std::inserter(svs, std::end(svs)),
				[](sys::state_value_id const& svid)
			{
				return svid.to_string();
			});
			return svs;
		}
	};
	get_system_svs_fn.add_dependency(ddl::node_dependency(ddl::node_ref{ sys_type }));
	for(auto const& entry : sys_defns)
	{
		get_system_svs_fn.add_dependency(entry.second->get_system_state_values_fn());
	}

	ddl::defn_node results = spc.enumeration("results")
		(ddl::define_enum_func{ get_system_svs_fn })
		;

	ddl::defn_node updates = spc.enumeration("updates")
		(ddl::define_enum_func{ get_system_svs_fn })
		;

	return spc.composite("sys_client_root")(ddl::define_children{}
		("sys_type", sys_type)
		("sys_details", sys_details)
		("results", results)
		("updates", updates)
		);
}





