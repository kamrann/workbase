// direct_bindings_ui_controller.h

#ifndef __NE_DIRECT_BINDINGS_UI_CONRTOLLER_H
#define __NE_DIRECT_BINDINGS_UI_CONRTOLLER_H

#include "ui_based_controller.h"
#include "controller_defn.h"

#include "ddl/ddl.h"


namespace sys
{

	class direct_bindings_ui_controller_defn:
		public i_controller_defn
	{
	public:
		// todo: this ugliness is to overcome the issue of the dep function not having yet been
		// initialized at the point of creating this defn component. named dep functions/triggers would probably
		// solve this issue.
		typedef std::function< ddl::dep_function< size_t >() > num_eff_provider_fn_t;

		direct_bindings_ui_controller_defn(num_eff_provider_fn_t num_eff_prov_fn);

	public:
		virtual std::string get_name() const override;
		virtual ddl::defn_node get_defn(ddl::specifier& spc) override;
		virtual controller_ptr create_controller(ddl::navigator nav) const override;

	protected:
		num_eff_provider_fn_t num_eff_prov_fn_;
	};

	class direct_bindings_ui_controller:
		public ui_based_controller
	{
	public:
//		static ddl::defn_node get_defn(ddl::specifier& spc, ddl::dep_function< size_t > df_num_effectors);
//		static std::unique_ptr< generic_ui_controller > generate(ddl::navigator nav, size_t num_effectors);

	public:
		direct_bindings_ui_controller(size_t num_effectors);

	protected:
		virtual double get_activation(unsigned long effector, input::input_stream_access const& input_data) const override;

	protected:
		friend class direct_bindings_ui_controller_defn;
	};

}


#endif

