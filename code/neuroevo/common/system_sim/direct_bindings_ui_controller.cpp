// direct_bindings_ui_controller.cpp

#include "direct_bindings_ui_controller.h"

#include "input_stream/wt_input_stream.h"
#include "input_stream/x360_input_stream.h"


namespace sys
{
	direct_bindings_ui_controller_defn::direct_bindings_ui_controller_defn(num_eff_provider_fn_t num_eff_prov_fn):
		num_eff_prov_fn_{ num_eff_prov_fn }
	{

	}

	std::string direct_bindings_ui_controller_defn::get_name() const
	{
		return "direct_ui_controller";
	}

	struct bound_element
	{
		input::InputElementType type;
		boost::variant <
			input::btn_element_id
			, input::axis_element_id
		> id;
	};

	ddl::defn_node direct_bindings_ui_controller_defn::get_defn(ddl::specifier& spc)//,
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

		auto available_btn_bindings = in_strm.available_buttons();
		auto available_axis_bindings = in_strm.available_axes();

		auto bnd_enum_vals_fn = ddl::enum_defn_node::enum_values_fn_t{ [available_btn_bindings, available_axis_bindings](ddl::navigator nav) -> ddl::enum_defn_node::enum_set_t
		{
			auto vals = ddl::enum_defn_node::enum_set_t{};
			for(size_t i = 0; i < available_btn_bindings.size(); ++i)
			{
				ddl::enum_defn_node::enum_value_t elem;
				elem.str = available_btn_bindings[i];
				elem.data = bound_element{
					input::InputElementType::Button,
					input::btn_element_id{ i }
				};
				vals.insert(std::end(vals), elem);
			}
			for(size_t i = 0; i < available_axis_bindings.size(); ++i)
			{
				ddl::enum_defn_node::enum_value_t elem;
				elem.str = available_axis_bindings[i];
				elem.data = bound_element{
					input::InputElementType::Axis,
					input::axis_element_id{ i }
				};
				vals.insert(std::end(vals), elem);
			}
			return vals;
		}
		};

		ddl::defn_node binding_sel = spc.enumeration("binding")
			(ddl::spc_range < size_t > { 1, 1 })
			(ddl::define_enum_func{ bnd_enum_vals_fn })
			;

		ddl::defn_node effector_binding = spc.composite("effector_binding")(ddl::define_children{}
			("binding", binding_sel)
			("effector", effector_sel)
			// todo: range mapping
			);

		return spc.composite("ui_controller")(ddl::define_children{}
			("effector_bindings", spc.list("effector_bindings")(ddl::spc_item{ effector_binding }))
			);
	}
	
	controller_ptr direct_bindings_ui_controller_defn::create_controller(ddl::navigator nav) const//, size_t num_effectors)
	{
		auto num_effectors = num_eff_prov_fn_()(nav);
		auto cont = std::make_unique< direct_bindings_ui_controller >(num_effectors);

		auto eff_bindings_nav = nav["effector_bindings"];
		auto count = eff_bindings_nav.list_num_items();
		for(size_t i = 0; i < count; ++i)
		{
			auto bnd_nav = eff_bindings_nav[i];
			auto input_binding = bnd_nav["binding"].get().as_single_enum_data< bound_element >();
			auto effector = bnd_nav["effector"].get().as_single_enum_data< size_t >();

			switch(input_binding.type)
			{
				case input::InputElementType::Button:
				cont->bind_button(boost::get< input::btn_element_id >(input_binding.id), effector);
				break;

				case input::InputElementType::Axis:
				cont->bind_axis(boost::get< input::axis_element_id >(input_binding.id), effector);
				break;
			}
		}

		return std::move(cont);
	}


	direct_bindings_ui_controller::direct_bindings_ui_controller(size_t num_effectors):
		ui_based_controller{ num_effectors }
	{

	}

	double direct_bindings_ui_controller::get_activation(unsigned long effector, input::input_stream_access const& input_data) const
	{
		auto it_b = bound_btns_.find(effector);
		if(it_b != bound_btns_.end())
		{
			// button
			auto btn_id = it_b->second;
			return input_data[btn_id].st == input::btn_element_state::ButtonState::Pressed ? 1.0 : 0.0;
		}
		
		auto it_a = bound_axes_.find(effector);
		if(it_a != bound_axes_.end())
		{
			// axis
			auto axis_id = it_a->second;
			return input_data[axis_id].value;
		}

		return 0.0;
	}

}



