// mlp.cpp

#include "mlp.h"
#include "rtp_system.h"
#include "../rtp_fixednn_genome_mapping.h"

#include "neuralnet/interface/output.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include <Wt/WComboBox>

#include <Box2D/Box2D.h>


namespace sys {

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

/*		(*provider)[path + std::string("mlp_num_layers")] = [](prm::param_accessor)
		{
			auto s = sb::integer("mlp_num_layers", 3, 2, 5);
			sb::label(s, "Num Layers");
			return s;
		};
*/
		path += std::string("hidden_layer_list");

		(*provider)[path + std::string("hl_neuron_count")] = [=](prm::param_accessor param_vals)
		{
			auto const num_inputs = param_vals["mlp_inputs"].size();
			auto s = sb::integer("hl_neuron_count", std::max(num_inputs / 2, 1u), 1);
			sb::label(s, "# Neurons");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::repeating_list("hidden_layer_list", "hl_neuron_count");
			sb::label(s, "Hidden Layers");
			return s;
		};
		
		path.pop();

		std::vector< std::pair< std::string, std::string > > fn_enum_values;
		for(size_t fn = 0; fn < (size_t)nnet::ActivationFnType::Count; ++fn)
		{
			fn_enum_values.emplace_back(std::make_pair(
				std::to_string(fn),
				std::string{ nnet::ActivationFnNames[fn] }
				));
		}

		(*provider)[path + std::string("hidden_activation_fn")] = [fn_enum_values](prm::param_accessor)
		{
			auto s = sb::enum_selection("hidden_activation_fn", fn_enum_values, 1, 1);
			sb::label(s, "Hidden Activation");
			return s;
		};

		(*provider)[path + std::string("hidden_steepness")] = [](prm::param_accessor)
		{
			auto s = sb::real("hidden_steepness", 0.5, 0.0);
			sb::label(s, "Hidden Steepness");
			return s;
		};

		(*provider)[path + std::string("output_activation_fn")] = [fn_enum_values](prm::param_accessor)
		{
			auto s = sb::enum_selection("output_activation_fn", fn_enum_values, 1, 1);
			sb::label(s, "Output Activation");
			return s;
		};

		(*provider)[path + std::string("output_steepness")] = [](prm::param_accessor)
		{
			auto s = sb::real("output_steepness", 0.5, 0.0);
			sb::label(s, "Output Steepness");
			return s;
		};


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("mlp_inputs"))(param_vals));
//			sb::append(s, provider->at(path + std::string("mlp_num_layers"))(param_vals));
			sb::append(s, provider->at(path + std::string("hidden_layer_list"))(param_vals));
			// TODO: if(num hidden layers > 0)
			sb::append(s, provider->at(path + std::string("hidden_activation_fn"))(param_vals));
			sb::append(s, provider->at(path + std::string("hidden_steepness"))(param_vals));
			//
			sb::append(s, provider->at(path + std::string("output_activation_fn"))(param_vals));
			sb::append(s, provider->at(path + std::string("output_steepness"))(param_vals));
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

		// NOTE: Since YAML map nodes do not retain order, need to use a map here keyed on repeat instance number
		// to ensure that hidden layers are extracted in the correct order.
		std::map< unsigned int, size_t > hidden_layer_sizes;
		auto hidden_list_node = param["hidden_layer_list"];
		for(auto const& node : hidden_list_node)
		{
			auto inst_num = node.first.as< unsigned int >();
			auto rel = prm::qualified_path{ "hidden_layer_list" };
			rel.leaf().set_index(inst_num);
			param.push_relative_path(rel);

			hidden_layer_sizes[inst_num] = param["hl_neuron_count"].as< size_t >();

			param.pop_relative_path();
		}
		//		int num_layers = param["mlp_num_layers"].as< int >();
		auto const num_layers = 2 + hidden_layer_sizes.size();

		// TODO: convert template
		nnet::ActivationFnType hidden_fn = (nnet::ActivationFnType)std::stoi(
			param["hidden_activation_fn"][0].as< std::string >());
		auto hidden_steepness = param["hidden_steepness"].as< double >();
		nnet::ActivationFnType output_fn = (nnet::ActivationFnType)std::stoi(
			param["output_activation_fn"][0].as< std::string >());
		auto output_steepness = param["output_steepness"].as< double >();

		size_t const NumNNInputs = inputs.size();

		auto sys_type = param["sys_type"][0].as< SystemType >();
		auto spec_type = compatible_node[0];
		size_t const NumNNOutputs = i_system::get_agent_num_effectors(sys_type, spec_type);

		//size_t const NumPerHidden = (NumNNInputs + NumNNOutputs + 1) / 2;

		nnet::mlp::layer_counts_t layer_counts;
		layer_counts.push_back(NumNNInputs);
		for(auto const& hl : hidden_layer_sizes)
		{
			layer_counts.push_back(hl.second);
		}
		layer_counts.push_back(NumNNOutputs);

		// TODO: activation function and steepness
		std::get< 0 >(result) = std::make_unique< fixednn_genome_mapping >(
/*			num_layers,
			NumNNInputs,
			NumNNOutputs,
			NumPerHidden
*/
			layer_counts
			);

		std::get< 1 >(result) = std::make_unique< mlp_controller_factory >(
			layer_counts,
			hidden_fn,
			hidden_steepness,
			output_fn,
			output_steepness,
			inputs
			);

		param.pop_relative_path();

		return result;
	}


	mlp_controller::mlp_controller(
		nnet::mlp::layer_counts_t const& _layer_counts,
		nnet::activation_function const& _hidden_fn,
		nnet::activation_function const& _output_fn,
		agent_sensor_list const& _inputs
		):
//		layer_counts(_layer_counts),
		inputs(_inputs)
	{
		assert(!inputs.empty());
		assert(_layer_counts.size() >= 2);

		nn.create(_layer_counts);

		for(size_t layer = 1; layer < _layer_counts.size() - 1; ++layer)
		{
			nn.set_activation_function_layer(layer, _hidden_fn);
		}
		nn.set_activation_function_layer(_layer_counts.size() - 1, _output_fn);
	}

	auto mlp_controller::get_input_ids() const -> input_id_list_t
	{
		return inputs;
	}
	
	auto mlp_controller::process(input_list_t const& inputs) -> output_list_t
	{
		// TODO: Does this actually result in a move?
		return nn.execute(inputs).data();
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
		nnet::mlp::layer_counts_t const& layer_counts,
		nnet::ActivationFnType hidden_fn,
		double hidden_steepness,
		nnet::ActivationFnType output_fn,
		double output_steepness,
		std::vector< std::string > const& inputs
		):
		m_layer_counts{ layer_counts },
		m_hidden_fn{ hidden_fn, hidden_steepness },
		m_output_fn{ output_fn, output_steepness },
		m_inputs{ inputs }
	{}

	std::unique_ptr< i_controller > mlp_controller_factory::create(i_agent const* attached_agent) const
	{
		auto m_mapped_inputs = attached_agent->get_mapped_inputs(m_inputs);
		return std::make_unique< mlp_controller >(
			m_layer_counts,
			m_hidden_fn,
			m_output_fn,
			m_mapped_inputs
			);
	}
}


