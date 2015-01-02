// evolved_controller_defn.cpp

#include "evolved_controller_defn.h"
#include "ffnn_ev_controller_defn.h"
#include "rnn_ev_controller_defn.h"

#include "system_sim/controller.h"


namespace sys {
	namespace ev {

		std::string evolvable_controller_defn::get_name() const
		{
			return "evolved";	// TODO:?
		}
		
		ddl::defn_node evolvable_controller_defn::get_defn(ddl::specifier& spc)
		{
			return spc.composite("ev_controller")
				;
		}

		controller_ptr evolvable_controller_defn::create_controller(ddl::navigator nav) const
		{
			// TODO: Not sure about this
			return nullptr;
		}



		ddl::defn_node evolvable_controller_impl_defn::get_defn(ddl::specifier& spc)
		{
			return ev_controller_type_defn.get_defn(spc);
		}
			
		std::unique_ptr< i_genetic_mapping > evolvable_controller_impl_defn::generate(ddl::navigator nav) const
		{
			return ev_controller_type_defn.generate(nav);
		}

		evolvable_controller_impl_defn::evolvable_controller_impl_defn(sys_defn_fn_t& sys_defn_fn, state_vals_fn_t& sv_fn)
		{
			ev_controller_type_defn.register_option("ffnn",
				std::make_shared< ffnn_ev_controller_defn >(sys_defn_fn, sv_fn));
			ev_controller_type_defn.register_option("rnn",
				std::make_shared< rnn_ev_controller_defn >(sys_defn_fn, sv_fn));
		}

	}
}




