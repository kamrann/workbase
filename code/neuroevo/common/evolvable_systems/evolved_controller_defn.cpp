// evolved_controller_defn.cpp

#include "evolved_controller_defn.h"
#include "ffnn_ev_controller_defn.h"

#include "system_sim/controller.h"

#include "params/schema_builder.h"
#include "params/param_accessor.h"


namespace sb = prm::schema;

namespace sys {
	namespace ev {

		std::string evolvable_controller_defn::get_name() const
		{
			return "evolved";	// TODO:?
		}
		
		std::string evolvable_controller_defn::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				return s;
			};

			return path.leaf().name();
		}

		controller_ptr evolvable_controller_defn::create_controller(prm::param_accessor acc) const
		{
			// TODO: Not sure about this
			return nullptr;
		}



		void evolvable_controller_impl_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			ev_controller_type_defn.update_schema_provider(provider, prefix);
		}
			
		std::unique_ptr< i_genetic_mapping > evolvable_controller_impl_defn::generate(prm::param_accessor acc) const
		{
			return ev_controller_type_defn.generate(acc);
		}

		evolvable_controller_impl_defn::evolvable_controller_impl_defn(sys_defn_fn_t sys_defn_fn)
		{
			ev_controller_type_defn.register_option("ffnn", std::make_shared< ffnn_ev_controller_defn >(sys_defn_fn));
		}

	}
}




