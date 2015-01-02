// interactive_controller.cpp

#include "interactive_controller.h"
#include "interactive.h"
#include "system.h"

//#include "params/param_accessor.h"
//#include "params/schema_builder.h"

#include "util/stl_util.hpp"

// TODO: Ideally get wt out of this library
#include <Wt/WGlobal>


namespace sys {

	const std::vector< std::pair< Wt::Key, std::string > > available_bindings_ = {
			{ Wt::Key_Enter, "Enter" },
			{ Wt::Key_Tab, "Tab" },
			{ Wt::Key_Backspace, "Backspace" },
			{ Wt::Key_Shift, "Shift" },
			{ Wt::Key_Control, "Control" },
			{ Wt::Key_Alt, "Alt" },
			{ Wt::Key_PageUp, "PageUp" },
			{ Wt::Key_PageDown, "PageDown" },
			{ Wt::Key_End, "End" },
			{ Wt::Key_Home, "Home" },
			{ Wt::Key_Left, "Left" },
			{ Wt::Key_Up, "Up" },
			{ Wt::Key_Right, "Right" },
			{ Wt::Key_Down, "Down" },
			{ Wt::Key_Insert, "Insert" },
			{ Wt::Key_Delete, "Delete" },
			{ Wt::Key_Escape, "Escape" },
			{ Wt::Key_F1, "F1" },
			{ Wt::Key_F2, "F2" },
			{ Wt::Key_F3, "F3" },
			{ Wt::Key_F4, "F4" },
			{ Wt::Key_F5, "F5" },
			{ Wt::Key_F6, "F6" },
			{ Wt::Key_F7, "F7" },
			{ Wt::Key_F8, "F8" },
			{ Wt::Key_F9, "F9" },
			{ Wt::Key_F10, "F10" },
			{ Wt::Key_F11, "F11" },
			{ Wt::Key_F12, "F12" },
			{ Wt::Key_Space, "Space" },
			{ Wt::Key_A, "A" },
			{ Wt::Key_B, "B" },
			{ Wt::Key_C, "C" },
			{ Wt::Key_D, "D" },
			{ Wt::Key_E, "E" },
			{ Wt::Key_F, "F" },
			{ Wt::Key_G, "G" },
			{ Wt::Key_H, "H" },
			{ Wt::Key_I, "I" },
			{ Wt::Key_J, "J" },
			{ Wt::Key_K, "K" },
			{ Wt::Key_L, "L" },
			{ Wt::Key_M, "M" },
			{ Wt::Key_N, "N" },
			{ Wt::Key_O, "O" },
			{ Wt::Key_P, "P" },
			{ Wt::Key_Q, "Q" },
			{ Wt::Key_R, "R" },
			{ Wt::Key_S, "S" },
			{ Wt::Key_T, "T" },
			{ Wt::Key_U, "U" },
			{ Wt::Key_V, "V" },
			{ Wt::Key_W, "W" },
			{ Wt::Key_X, "X" },
			{ Wt::Key_Y, "Y" },
			{ Wt::Key_Z, "Z" },
	};

	std::vector< std::pair< std::string, std::string > > i_interactive_controller::get_available_bindings()
	{
		std::vector< std::pair< std::string, std::string > > bindings;
		std::transform(
			std::begin(available_bindings_),
			std::end(available_bindings_),
			std::inserter(bindings, std::end(bindings)),
			[](std::pair< Wt::Key, std::string > const& in)
		{
			return std::make_pair(
				std::to_string((int)in.first),
				in.second);
		});
		return bindings;
	}

#if 0
	namespace sb = prm::schema;

	std::string i_interactive_controller::update_direct_bindings_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "ic_direct_bindings" };
		auto path = prefix + relative;

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::border(s, std::string{ "Direct Bindings" });

			auto agent_type_node = i_controller::get_controller_agent_type(param_vals);
			auto sys_type = param_vals["sys_type"].as< SystemType >();
			auto num_effectors = i_system::get_agent_num_effectors(sys_type, agent_type_node);
			auto bindings = get_available_bindings();
			for(size_t eff = 0; eff < num_effectors; ++eff)
			{
				auto name = std::string{ "Effector " } + std::to_string(eff + 1);
				auto binder = sb::enum_selection(
					name,
					bindings,
					0, 1);
				sb::append(s, binder);
			}

			return s;
		};

		return relative;
	}
#endif

#if 0
	std::unique_ptr< i_controller_factory > i_interactive_controller::create_factory(prm::param_accessor param_vals)
	{
		auto factory_fn = [=](i_agent const*) -> std::unique_ptr< i_controller >
		{
			return interactive_controller::create_instance(param_vals);
		};

		return std::make_unique< controller_factory >(factory_fn, true);
	}
#endif
}



