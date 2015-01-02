// generic_ui_controller.cpp

#include "generic_ui_controller.h"

#include "input_stream/wt_input_stream.h"
#include "input_stream/x360_input_stream.h"


namespace sys
{
	generic_ui_controller_defn::generic_ui_controller_defn(num_eff_provider_fn_t num_eff_prov_fn):
		num_eff_prov_fn_{ num_eff_prov_fn }
	{

	}

	std::string generic_ui_controller_defn::get_name() const
	{
		return "generic_ui_controller";
	}

	ddl::defn_node generic_ui_controller_defn::get_defn(ddl::specifier& spc)//,
//		ddl::dep_function< size_t > df_num_effectors)	// todo: named effector bindings
	{
		auto df_num_effectors = num_eff_prov_fn_();

		auto eff_enum_vals_fn = ddl::enum_defn_node::enum_values_fn_t{ [df_num_effectors](ddl::navigator nav) -> ddl::enum_defn_node::enum_set_t
		{
			auto vals = ddl::enum_defn_node::enum_set_t{};
			auto count = df_num_effectors(nav);
			for(size_t eff = 0; eff < count; ++eff)
			{
				ddl::enum_defn_node::enum_value_t elem;
				elem.str = std::string{ "effector_" } +std::to_string(eff + 1);
				elem.data = eff;
				vals.insert(std::end(vals), elem);
			}
			return vals;
		}
		};
		eff_enum_vals_fn.add_dependency(df_num_effectors);

		ddl::defn_node effector_sel = spc.enumeration("effector")
			(ddl::spc_range < size_t > { 1, 1 })
			(ddl::define_enum_func{ eff_enum_vals_fn })
			;

		// TODO: For now just hard coding input stream type
//		input::wt_input_stream in_strm;
		input::x360_input_stream in_strm;

		auto available_bindings = in_strm.available_buttons();

		auto bnd_enum_vals_fn = ddl::enum_defn_node::enum_values_fn_t{ [available_bindings](ddl::navigator nav) -> ddl::enum_defn_node::enum_set_t
		{
			auto vals = ddl::enum_defn_node::enum_set_t{};
			for(size_t i = 0; i < available_bindings.size(); ++i)
			{
				ddl::enum_defn_node::enum_value_t elem;
				elem.str = available_bindings[i];
				elem.data = input::btn_element_id{ i };
				vals.insert(std::end(vals), elem);
			}
			return vals;
		}
		};

		ddl::defn_node binding_sel = spc.enumeration("binding")
			(ddl::spc_range < size_t > { 1, 1 })
			(ddl::define_enum_func{ bnd_enum_vals_fn })
			;

		ddl::defn_node modulation_binding = spc.composite("modulation_binding")(ddl::define_children{}
			("binding", binding_sel)
			("modulation", spc.realnum("modulation")(ddl::spc_range < ddl::realnum_defn_node::value_t > { -1.0, 1.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 }))
			);

		ddl::defn_node effector_binding = spc.composite("effector_binding")(ddl::define_children{}
			("binding", binding_sel)
			("effector", effector_sel)
			);

		return spc.composite("ui_controller")(ddl::define_children{}
			("effector_bindings", spc.list("effector_bindings")(ddl::spc_item{ effector_binding }))
			("modulation_bindings", spc.list("modulation_bindings")(ddl::spc_item{ modulation_binding }))
			);
	}
	
	controller_ptr generic_ui_controller_defn::create_controller(ddl::navigator nav) const//, size_t num_effectors)
	{
		auto num_effectors = num_eff_prov_fn_()(nav);
		auto cont = std::make_unique< generic_ui_controller >(num_effectors);

		auto eff_bindings_nav = nav["effector_bindings"];
		auto count = eff_bindings_nav.list_num_items();
		for(size_t i = 0; i < count; ++i)
		{
			auto bnd_nav = eff_bindings_nav[i];
			auto input_binding = bnd_nav["binding"].get().as_single_enum_data< input::btn_element_id >();
			auto effector = bnd_nav["effector"].get().as_single_enum_data< size_t >();

			cont->bind_button(input_binding, effector);
		}

		auto mod_bindings_nav = nav["modulation_bindings"];
		count = mod_bindings_nav.list_num_items();
		for(size_t i = 0; i < count; ++i)
		{
			auto bnd_nav = mod_bindings_nav[i];
			auto input_binding = bnd_nav["binding"].get().as_single_enum_data< input::btn_element_id >();
			auto modulation = bnd_nav["modulation"].get().as_real();

			cont->modulations_[input_binding] = modulation;
		}

		return std::move(cont);
	}


	generic_ui_controller::generic_ui_controller(size_t num_effectors):
		ui_based_controller{ num_effectors }
	{

	}

	double generic_ui_controller::get_current_modulation(input::input_stream_access const& input_data) const
	{
		// NOTE: Assuming only one modulation key is pressed at a time
		for(auto const& mod : modulations_)
		{
			if(input_data[mod.first].st == input::btn_element_state::ButtonState::Pressed)
			{
				return mod.second;
			}
		}
		return 0.0;
	}

	double generic_ui_controller::get_activation(unsigned long effector, input::input_stream_access const& input_data) const
	{
		auto mod = get_current_modulation(input_data);
		auto btn_id = bound_btns_.at(effector);
		return input_data[btn_id].st == input::btn_element_state::ButtonState::Pressed ? mod : 0;
	}

}



