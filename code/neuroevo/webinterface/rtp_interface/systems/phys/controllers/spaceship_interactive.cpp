// spaceship_interactive.cpp

#include "spaceship_interactive.h"
#include "../../interactive.h"

#include "wt_param_widgets/param_accessor.h"
#include "wt_param_widgets/schema_builder.h"


namespace rtp
{
	namespace sb = prm::schema;

	// TODO: Take an extra parameter to support different thruster setups
	std::string spaceship_interactive_controller::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "spaceship_bindings" };
		auto path = prefix + relative;

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::border(s, std::string{ "Further Bindings" });

			auto bindings = i_interactive_controller::get_available_bindings();

			auto rot_anticw = sb::enum_selection(
				"rotate_anticw",
				bindings,
				0, 1);
			sb::label(rot_anticw, "Rotate Anti-CW");
			sb::append(s, rot_anticw);
			
			auto rot_cw = sb::enum_selection(
				"rotate_cw",
				bindings,
				0, 1);
			sb::label(rot_cw, "Rotate CW");
			sb::append(s, rot_cw);

			auto forwards = sb::enum_selection(
				"forwards",
				bindings,
				0, 1);
			sb::label(forwards, "Forwards");
			sb::append(s, forwards);

			auto backwards = sb::enum_selection(
				"backwards",
				bindings,
				0, 1);
			sb::label(backwards, "Backwards");
			sb::append(s, backwards);

			auto left = sb::enum_selection(
				"left",
				bindings,
				0, 1);
			sb::label(left, "Left");
			sb::append(s, left);

			auto right = sb::enum_selection(
				"right",
				bindings,
				0, 1);
			sb::label(right, "Right");
			sb::append(s, right);

			return s;
		};

		return relative;
	}

	std::unique_ptr< i_controller_factory > spaceship_interactive_controller::create_factory(prm::param_accessor param)
	{
		auto factory_fn = [=](i_agent const*) -> std::unique_ptr< i_controller >
		{
			auto controller = interactive_controller::create_instance(param);
			
			// TODO: Currently hard coded for "square_complete" thruster configuration!!!!!!!!!!!!!!!!!!!!!!!!!
			auto binding = interactive_controller::extract_input_binding(param["rotate_anticw"]);
			if(binding)
			{
				controller->add_binding(*binding, 2);
				controller->add_binding(*binding, 3);
				controller->add_binding(*binding, 4);
				controller->add_binding(*binding, 5);
			}

			binding = interactive_controller::extract_input_binding(param["rotate_cw"]);
			if(binding)
			{
				controller->add_binding(*binding, 0);
				controller->add_binding(*binding, 1);
				controller->add_binding(*binding, 6);
				controller->add_binding(*binding, 7);
			}

			binding = interactive_controller::extract_input_binding(param["forwards"]);
			if(binding)
			{
				controller->add_binding(*binding, 5);
				controller->add_binding(*binding, 7);
			}

			binding = interactive_controller::extract_input_binding(param["backwards"]);
			if(binding)
			{
				controller->add_binding(*binding, 4);
				controller->add_binding(*binding, 6);
			}

			binding = interactive_controller::extract_input_binding(param["left"]);
			if(binding)
			{
				controller->add_binding(*binding, 1);
				controller->add_binding(*binding, 2);
			}

			binding = interactive_controller::extract_input_binding(param["right"]);
			if(binding)
			{
				controller->add_binding(*binding, 0);
				controller->add_binding(*binding, 3);
			}

			return std::move(controller);
		};

		return std::make_unique< controller_factory >(factory_fn, true);
	}

}

