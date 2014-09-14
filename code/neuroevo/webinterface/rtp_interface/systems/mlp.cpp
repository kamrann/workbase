// mlp.cpp

#include "mlp.h"
#include "rtp_system.h"
#include "../rtp_fixednn_genome_mapping.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include <Wt/WComboBox>

#include <Box2D/Box2D.h>


namespace rtp {

	namespace sb = prm::schema;

	std::string mlp_controller::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "mlp_controller" };
		auto path = prefix + relative;

		(*provider)[path + std::string("mlp_inputs")] = [](prm::param_accessor param_vals)
		{
			auto compatible_node = param_vals["compatible_agent_types"];

			bool is_source = compatible_node.size() > 0;
			////// TODO: Temp, while this is limited to single selection
			is_source = is_source && !compatible_node.as< prm::is_unspecified >();
			//////

			auto num_available_inputs = 0;
			std::vector< std::pair< std::string, std::string > > enum_values;
			if(is_source)
			{
				// TODO: For now assuming single selection
				//auto agent_type = compatible_node[0].as< agent_body_spec::Type >();
				//auto input_names = agent_body_spec::sensor_inputs(agent_type);

				auto sys_type = param_vals["sys_type"][0].as< SystemType >();
				auto input_names = i_system::get_agent_sensor_names(sys_type, compatible_node[0], param_vals);

				num_available_inputs = input_names.size();
				enum_values.resize(num_available_inputs);
				int idx = 0;
				std::generate(begin(enum_values), end(enum_values), [&input_names, &idx]()
				{
					// TODO: If allow different agent types to be linked to the same controller,
					// then cannot assume that the index in the input name list corresponds to agent_sensor_id,
					// since this list will just be an intsersection of the input lists of each type.
					auto res = std::make_pair(
						input_names[idx],//std::to_string(idx),
						input_names[idx]);
					++idx;
					return res;
				});
			}

			auto s = sb::enum_selection("mlp_inputs", enum_values, 1, num_available_inputs);
			sb::label(s, "Inputs");
			sb::update_on(s, "evolved_controller_compatibility_changed");
			return s;
		};

		(*provider)[path + std::string("mlp_num_layers")] = [](prm::param_accessor)
		{
			auto s = sb::integer("mlp_num_layers", 3, 2, 5);
			sb::label(s, "Num Layers");
			return s;
		};


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("mlp_inputs"))(param_vals));
			sb::append(s, provider->at(path + std::string("mlp_num_layers"))(param_vals));
			return s;
		};

		return relative;
	}

	std::tuple<
		std::unique_ptr< i_genome_mapping >,
		std::unique_ptr< i_controller_factory >
	> mlp_controller::create_instance_evolvable(prm::param_accessor param)
	{
		auto result = std::tuple< std::unique_ptr< i_genome_mapping >, std::unique_ptr< i_controller_factory > > ();

		param.push_relative_path(prm::qualified_path("mlp_controller"));
		//		auto type = prm::find_value(param, "Inputs").as< Type >();

		auto compatible_node = param["compatible_agent_types"];
		if(compatible_node.as< prm::is_unspecified >())
		{
			throw prm::required_unspecified("Evolvable Controller Compatible Agents");
		}

		auto ep = param["mlp_inputs"].as< prm::enum_param_val >();
		auto inputs = boost::any_cast<std::vector< std::string >>(ep.contents);

		int num_layers = param["mlp_num_layers"].as< int >();

		size_t const NumNNInputs = inputs.size();

		auto sys_type = param["sys_type"][0].as< SystemType >();
		auto spec_type = compatible_node[0];
		size_t const NumNNOutputs = //agent_body_spec::num_effectors(spec_type);
			i_system::get_agent_num_effectors(sys_type, spec_type);

		size_t const NumPerHidden = (NumNNInputs + NumNNOutputs + 1) / 2;

		std::get< 0 >(result) = std::make_unique< fixednn_genome_mapping >(
			num_layers,
			NumNNInputs,
			NumNNOutputs,
			NumPerHidden);

		std::get< 1 >(result) = std::make_unique< mlp_controller_factory >(
			inputs,
			num_layers,
			NumPerHidden,
			NumNNOutputs);

		param.pop_relative_path();

		return result;
	}


	mlp_controller::mlp_controller(agent_sensor_list const& _inputs,
		size_t num_layers, size_t per_hidden, size_t num_outputs):
		inputs(_inputs),
		num_nn_layers(num_layers), num_per_hidden(per_hidden), num_nn_outputs(num_outputs)
	{
		assert(!inputs.empty());
		assert(num_nn_layers >= 2);
		assert(num_per_hidden >= 1);
		assert(num_nn_outputs >= 1);

		std::vector< unsigned int > layer_neurons(num_nn_layers, num_per_hidden);
		layer_neurons[0] = inputs.size();
		layer_neurons[num_nn_layers - 1] = num_nn_outputs;
		nn.create_standard_array(num_nn_layers, &layer_neurons[0]);

		nn.set_activation_steepness_hidden(1.0);
		nn.set_activation_steepness_output(1.0);

		nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
		// TODO: variable by constructor arg - eg. THRESHOLD for spaceship thrusters
		// of course, could just perform this manually in the body::activate_effectors() method
		nn.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);
	}

	auto mlp_controller::get_input_ids() const -> input_id_list_t
	{
		return inputs;
	}
	
	auto mlp_controller::process(input_list_t const& inputs) -> output_list_t
	{
		double* nn_outputs = nn.run((double*)&inputs[0]);
		return{ nn_outputs, nn_outputs + num_nn_outputs };
	}

/*
	std::vector< double > mlp_controller::map_nn_inputs(state_t const& st)
	{
		std::vector< double > nni(inputs.size());
		auto in_it = begin(inputs);
		auto mapped_in_it = begin(nni);
		for(; mapped_in_it != end(nni); ++in_it, ++mapped_in_it)
		{
#if 0 TODO:
			*mapped_in_it = st.body.get()->get_sensor_value(*in_it);
#endif
		}
				
		return nni;
	}

	void mlp_controller::update(phys_system::state& st, phys_system::scenario_data sdata)
	{
		std::vector< double > nn_inputs = map_nn_inputs(st);
		double* nn_outputs = nn.run((double*)&nn_inputs[0]);
		std::vector< double > activations(num_nn_outputs);
		std::copy(nn_outputs, nn_outputs + num_nn_outputs, begin(activations));
#if 0 TODO:
		st.body.get()->activate_effectors(activations);
#endif
	}
*/	
	mlp_controller_factory::mlp_controller_factory(
//		agent_sensor_list const& inputs, 
		std::vector< std::string > const& inputs,
		size_t num_layers, size_t num_per_hidden, size_t num_outputs):
		m_inputs(inputs),
		m_num_nn_layers(num_layers),
		m_num_per_hidden(num_per_hidden),
		m_num_nn_outputs(num_outputs)
	{}

	std::unique_ptr< i_controller > mlp_controller_factory::create(i_agent const* attached_agent) const
	{
		auto m_mapped_inputs = attached_agent->get_mapped_inputs(m_inputs);
		return std::make_unique< mlp_controller >(
			m_mapped_inputs,
			m_num_nn_layers,
			m_num_per_hidden,
			m_num_nn_outputs);
	}
}


