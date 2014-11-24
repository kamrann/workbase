// ffnn_ev_controller_defn.cpp

#include "ffnn_ev_controller_defn.h"
#include "fixed_ffnn_genetic_mapping.h"

#include "system_sim/system_defn.h"
#include "system_sim/system_state_values.h"

#include "genetic_algorithm/param_spec/real_valued_mutation_ps.h"

#include "neuralnet/interface/activation_functions.h"

#include "params/param.h"
#include "params/schema_builder.h"
#include "params/param_accessor.h"


namespace sb = prm::schema;

namespace sys {
	namespace ev {

		ffnn_ev_controller_defn::ffnn_ev_controller_defn(evolvable_controller_impl_defn::sys_defn_fn_t sys_defn_fn):
			sys_defn_fn_(sys_defn_fn)
		{
			gene_mutation_defn_.register_option(
				"linear",
				ga::linear_real_gene_mutation_defn< double >::update_schema_provider,
				ga::linear_real_gene_mutation_defn< double >::generate
				);
			gene_mutation_defn_.register_option(
				"gaussian",
				ga::gaussian_real_gene_mutation_defn< double >::update_schema_provider,
				ga::gaussian_real_gene_mutation_defn< double >::generate
				);
			gene_mutation_defn_.register_option(
				"variable_gaussian",
				ga::variable_gaussian_real_gene_mutation_defn< double >::update_schema_provider,
				ga::variable_gaussian_real_gene_mutation_defn< double >::generate
				);
		}

		void ffnn_ev_controller_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			(*provider)[path + std::string("ffnn_inputs")] = [this](prm::param_accessor param_vals)
			{
				auto sys_defn = sys_defn_fn_(param_vals);

				// Get a list of agents using this evolved controller
				// TODO: !!! this. process will be similar to below in generate() where we find num_outputs
				//auto attached_agents = get_attached_agents();

				// Get list of available state values
				// TODO: For now assuming just 1 agent, but ideally want to use the intersection of state values
				// for each attached agent.
				// TODO: Should be using get_agent_sensor_names(), or some agent-relative call
				auto available_inputs = sys_defn->get_system_state_values(param_vals);
				auto num_available_inputs = available_inputs.size();
				std::vector< std::pair< std::string, std::string > > enum_values;
				if(num_available_inputs > 0)
				{
					enum_values.resize(num_available_inputs);
					int idx = 0;
					std::generate(begin(enum_values), end(enum_values), [&available_inputs, &idx]()
					{
						auto res = std::make_pair(
							available_inputs[idx].to_string(),
							available_inputs[idx].to_string()
							);
						++idx;
						return res;
					});
				}

				auto s = sb::enum_selection("ffnn_inputs", enum_values, 1, num_available_inputs);
				//sb::label(s, "Inputs");
				//sb::update_on(s, "evolved_controller_compatibility_changed");
				return s;
			};

			path += std::string("hidden_layer_list");

			(*provider)[path + std::string("hl_neuron_count")] = [=](prm::param_accessor param_vals)
			{
				auto const num_inputs = prm::extract_as< prm::enum_param_val >(param_vals["ffnn_inputs"]).size();
				auto s = sb::integer("hl_neuron_count", std::max(num_inputs / 2, 1u), 1);
				//sb::label(s, "# Neurons");
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor param_vals)
			{
				auto s = sb::repeating_list("hidden_layer_list", "hl_neuron_count");
				//sb::label(s, "Hidden Layers");
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
				//sb::label(s, "Hidden Activation");
				return s;
			};

			(*provider)[path + std::string("hidden_steepness")] = [](prm::param_accessor)
			{
				auto s = sb::real("hidden_steepness", 0.5, 0.0);
				//sb::label(s, "Hidden Steepness");
				return s;
			};

			(*provider)[path + std::string("output_activation_fn")] = [fn_enum_values](prm::param_accessor)
			{
				auto s = sb::enum_selection("output_activation_fn", fn_enum_values, 1, 1);
				//sb::label(s, "Output Activation");
				return s;
			};

			(*provider)[path + std::string("output_steepness")] = [](prm::param_accessor)
			{
				auto s = sb::real("output_steepness", 0.5, 0.0);
				//sb::label(s, "Output Steepness");
				return s;
			};

			//////////TODO other location in param tree
			gene_mutation_defn_.update_schema_provider(provider, path + std::string{ "temp_mutation" });

			////////////////


			(*provider)[path] = [=](prm::param_accessor param_vals)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string("ffnn_inputs"))(param_vals));
				//			sb::append(s, provider->at(path + std::string("mlp_num_layers"))(param_vals));
				sb::append(s, provider->at(path + std::string("hidden_layer_list"))(param_vals));
				// TODO: if(num hidden layers > 0)
				sb::append(s, provider->at(path + std::string("hidden_activation_fn"))(param_vals));
				sb::append(s, provider->at(path + std::string("hidden_steepness"))(param_vals));
				//
				sb::append(s, provider->at(path + std::string("output_activation_fn"))(param_vals));
				sb::append(s, provider->at(path + std::string("output_steepness"))(param_vals));

				// TODO::
				sb::append(s, provider->at(path + std::string{ "temp_mutation" })(param_vals));
				return s;
			};
		}
			
		void ffnn_ev_controller_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, int component)
		{
			switch(component)
			{

			}
		}
			
		std::unique_ptr< i_genetic_mapping > ffnn_ev_controller_defn::generate(prm::param_accessor acc) const
		{
			state_value_id_list inputs;
			auto inputs_param = prm::extract_as< prm::enum_param_val >(acc["ffnn_inputs"]);
			for(auto const& str : inputs_param)
			{
				inputs.push_back(state_value_id::from_string(str));
			}

			std::vector< int > hidden_layer_counts;
			acc.move_to(acc.find_path("hidden_layer_list"));
			auto layer_paths = acc.children();
			for(auto const& p : layer_paths)
			{
				acc.move_to(p);

				auto count = prm::extract_as< int >(acc["hl_neuron_count"]);
				hidden_layer_counts.push_back(count);

				acc.revert();
			}
			acc.revert();

			auto sys_defn = sys_defn_fn_(acc);
			// TODO: following strings hard coded to match with 
			// evolved_system_domain_defn::register_system_type(), and
			// evolvable_controller_defn::get_name()
			auto connected_paths = sys_defn->get_connected_agent_specs("evolved_cls", "evolved", acc);
			// TODO: here assuming only 1 agent spec associated with the evolved controller
			auto connected_spec_acc = acc;
			connected_spec_acc.move_to(connected_paths[0]);
			size_t num_outputs = sys_defn->get_agent_num_effectors(connected_spec_acc);

			std::vector< size_t > layers;	// TODO: layer_counts_t should be part of nnet interface, not implementation
			layers.push_back(inputs.size());
			layers.insert(std::end(layers), std::begin(hidden_layer_counts), std::end(hidden_layer_counts));
			layers.push_back(num_outputs);

			auto gene_mut_fn = gene_mutation_defn_.generate(acc("temp_mutation"));	// TODO: 

			return std::make_unique< fixed_ffnn_genetic_mapping >(inputs, layers, gene_mut_fn);
		}

	}
}




