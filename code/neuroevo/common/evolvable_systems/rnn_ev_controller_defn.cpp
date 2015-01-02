// rnn_ev_controller_defn.cpp

#include "rnn_ev_controller_defn.h"
#include "fixed_rnn_genetic_mapping.h"

#include "system_sim/system_defn.h"
#include "system_sim/system_state_values.h"

#include "genetic_algorithm/param_spec/real_valued_mutation_ps.h"

#include "neuralnet/interface/activation_functions.h"


namespace sys {
	namespace ev {

		rnn_ev_controller_defn::rnn_ev_controller_defn(
			evolvable_controller_impl_defn::sys_defn_fn_t& sys_defn_fn,
			evolvable_controller_impl_defn::state_vals_fn_t& sv_fn):
			sv_fn_(sv_fn),
			sys_defn_fn_(sys_defn_fn)
		{
			gene_mutation_defn_.register_option(
				"linear",
				ga::linear_real_gene_mutation_defn< double >::get_defn,
				ga::linear_real_gene_mutation_defn< double >::generate
				);
			gene_mutation_defn_.register_option(
				"gaussian",
				ga::gaussian_real_gene_mutation_defn< double >::get_defn,
				ga::gaussian_real_gene_mutation_defn< double >::generate
				);
			gene_mutation_defn_.register_option(
				"variable_gaussian",
				ga::variable_gaussian_real_gene_mutation_defn< double >::get_defn,
				ga::variable_gaussian_real_gene_mutation_defn< double >::generate
				);
		}

		ddl::defn_node rnn_ev_controller_defn::get_defn(ddl::specifier& spc)
		{
			auto available_input_names_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this](ddl::navigator nav)
			{
//				auto sys_defn = sys_defn_fn_(nav);

				// Get a list of agents using this evolved controller
				// TODO: !!! this. process will be similar to below in generate() where we find num_outputs
				//auto attached_agents = get_attached_agents();

				// Get list of available state values
				// TODO: For now assuming just 1 agent, but ideally want to use the intersection of state values
				// for each attached agent.
				// TODO: Should be using get_agent_sensor_names(), or some agent-relative call
//				auto available_inputs = sys_defn->get_system_state_values(nav);	// todo: deps
				//auto num_available_inputs = available_inputs.size();

				auto available_inputs = sv_fn_(nav);

				ddl::enum_defn_node::enum_str_set_t enum_values;
				for(auto const& ip : available_inputs)
				{
					enum_values.insert(std::end(enum_values), ip.to_string());
				}
				return enum_values;
			}
			};
			available_input_names_fn.add_dependency(sv_fn_);// sys_defn_fn_);

			ddl::defn_node inputs = spc.enumeration("inputs")
				(ddl::spc_range < size_t > { 1, boost::none })
				(ddl::define_enum_func{ available_input_names_fn })
				;

			ddl::defn_node hidden_neuron_count = spc.integer("hidden_neuron_count")
				(ddl::spc_min < ddl::integer_defn_node::value_t > { 0 })
				(ddl::spc_default < ddl::integer_defn_node::value_t > { 1 })	// todo: dependent upon current sel count of inputs?
				;

			auto af_vals = ddl::define_enum_fixed{};
			for(size_t fn = 0; fn < (size_t)nnet::ActivationFnType::Count; ++fn)
			{
				af_vals = af_vals(std::string{ nnet::ActivationFnNames2[fn] });
			}

			ddl::defn_node act_fn = spc.enumeration("act_fn")
				(ddl::spc_range < size_t > { 1, 1 })
				(ddl::spc_default < ddl::enum_defn_node::str_value_t > { { nnet::ActivationFnNames2[(int)nnet::ActivationFnType::SigmoidSymmetric] } })
				(af_vals)
				;

			ddl::defn_node steepness = spc.realnum("steepness")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			//////////TODO other location in param tree
			ddl::defn_node temp_mutation = gene_mutation_defn_.get_defn(spc);

			return spc.composite("rnn_ev_controller")(ddl::define_children{}
				("inputs", inputs)
				("hidden_neuron_count", hidden_neuron_count)
				("hidden_act_fn", act_fn)
				("hidden_steepness", steepness)
				("output_act_fn", act_fn)
				("output_steepness", steepness)
				("temp_mutation", temp_mutation)
				);
		}
			
		std::unique_ptr< i_genetic_mapping > rnn_ev_controller_defn::generate(ddl::navigator nav) const
		{
			state_value_id_list inputs;
			auto inputs_param = nav["inputs"].get().as_enum_str();
			for(auto const& str : inputs_param)
			{
				inputs.push_back(state_value_id::from_string(str));
			}

			size_t hidden_count = nav["hidden_neuron_count"].get().as_int();

			auto sys_defn = sys_defn_fn_(nav);
			// TODO: following strings hard coded to match with 
			// evolved_system_domain_defn::register_system_type(), and
			// evolvable_controller_defn::get_name()
			auto connected_navs = sys_defn->get_connected_spec_navs_fn("evolved_cls", "evolved")(nav);
			// TODO: here assuming only 1 agent spec associated with the evolved controller
			auto connected_spec_nav = connected_navs[0]["details"][(size_t)0];
			size_t num_outputs = sys_defn->get_inst_num_effectors_fn()(nav);//connected_spec_nav);

			std::vector< size_t > layers;	// TODO: layer_counts_t should be part of nnet interface, not implementation
			layers.push_back(inputs.size());
			layers.push_back(hidden_count);
			layers.push_back(num_outputs);

			auto hidden_af = nnet::activation_function{
				nnet::ActivationFnNameMap.at(nav["hidden_act_fn"].get().as_single_enum_str()),
				nav["hidden_steepness"].get().as_real()
			};
			auto output_af = nnet::activation_function{
				nnet::ActivationFnNameMap.at(nav["output_act_fn"].get().as_single_enum_str()),
				nav["output_steepness"].get().as_real()
			};

			auto gene_mut_fn = gene_mutation_defn_.generate(nav["temp_mutation"]);	// TODO: 

			return std::make_unique< fixed_rnn_genetic_mapping >(inputs, layers, hidden_af, output_af, gene_mut_fn);
		}

	}
}




