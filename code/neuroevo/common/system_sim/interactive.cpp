// interactive.cpp

#include "interactive.h"

#include "params/param_accessor.h"
#include "params/param_yaml.h"


namespace sys
{
	boost::optional< unsigned long > interactive_controller::extract_input_binding(prm::param binding)
	{
		auto is_bound = !prm::is_unspecified(binding);
		if(is_bound)
		{
			auto input_as_string = prm::extract_as< prm::enum_param_val >(binding)[0];
			// Convert to key identifier
			auto input = std::stoul(input_as_string);
			return input;
		}
		else
		{
			return boost::none;
		}
	}

	std::unique_ptr< interactive_controller > interactive_controller::create_instance(prm::param_accessor param_vals)
	{
		auto bindings_path = param_vals.find_path("ic_direct_bindings");
		param_vals.move_to(bindings_path);
		auto bindings = param_vals.children();
		auto num_effectors = bindings.size();
		auto controller = std::make_unique< interactive_controller >(num_effectors);

		size_t eff_idx = 0;
		for(auto path : bindings)
		{
			auto binding_val = param_vals[path.leaf().name()];
			auto binding = extract_input_binding(binding_val);
			if(binding)
			{
				controller->add_binding(*binding, eff_idx);
			}

			++eff_idx;
		}

		param_vals.revert();
		return controller;
	}


	interactive_controller::interactive_controller(size_t num_effectors)
	{
		m_activations.resize(num_effectors, 0);
	}

	void interactive_controller::add_binding(unsigned long input, unsigned long effector)
	{
		m_bindings[input].push_back(effector);
		m_required_inputs.insert(input);
	}

	i_controller::input_id_list_t interactive_controller::get_input_ids() const
	{
		return input_id_list_t();
	}

	i_controller::output_list_t interactive_controller::process(input_list_t const&)
	{
		output_list_t results;
		std::transform(
			std::begin(m_activations),
			std::end(m_activations),
			std::inserter(results, std::end(results)),
			[](unsigned long val)
		{
			return val == 0 ? 0.0 : 1.0;
		});
		return results;
	}

	const interactive_input_set& interactive_controller::get_required_interactive_inputs() const
	{
		return m_required_inputs;
	}

	void interactive_controller::register_input(interactive_input const& input)
	{
		auto const& effectors = m_bindings[input.key_index];
		for(auto activation_idx : effectors)
		{
			if(input.down)
			{
				++m_activations[activation_idx];
			}
			else
			{
				--m_activations[activation_idx];
			}
		}
	}
}


