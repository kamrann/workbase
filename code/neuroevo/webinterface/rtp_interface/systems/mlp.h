// mlp.h

#ifndef __NE_RTP_MLP_CONTROLLER_H
#define __NE_RTP_MLP_CONTROLLER_H

#include "rtp_controller.h"
#include "rtp_agent.h"
#include "../rtp_mlp_controller.h"

#include "neuralnet/interface/activation_functions.h"

#include "wt_param_widgets/pw_fwd.h"


namespace rtp
{
	class i_genome_mapping;

	class mlp_controller:
		public i_controller,
		public generic_mlp_controller
	{
	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		static std::tuple<
			std::unique_ptr< i_genome_mapping >,
			std::unique_ptr< i_controller_factory >
		> create_instance_evolvable(prm::param_accessor param);

	public:
		mlp_controller(
			nnet::mlp::layer_counts_t const& _layer_counts,
			nnet::activation_function const& _hidden_fn,
			nnet::activation_function const& _output_fn,
			agent_sensor_list const& _inputs
			);

	public:
		virtual input_id_list_t get_input_ids() const;
		virtual output_list_t process(input_list_t const& inputs);

	protected:
//		nnet::mlp::layer_counts_t const layer_counts;

		agent_sensor_list inputs;
	};


	class mlp_controller_factory: public i_controller_factory
	{
	public:
		mlp_controller_factory(
			nnet::mlp::layer_counts_t const& layer_counts,
			nnet::ActivationFnType hidden_fn,
			double hidden_steepness,
			nnet::ActivationFnType output_fn,
			double output_steepness,
			std::vector< std::string > const& inputs
			);

	public:
		virtual std::unique_ptr< i_controller > create(i_agent const* attached_agent) const override;
		virtual bool is_interactive() const override
		{
			return false;
		}

	private:
		nnet::mlp::layer_counts_t const m_layer_counts;
		nnet::activation_function const m_hidden_fn;
		nnet::activation_function const m_output_fn;
		std::vector< std::string > const m_inputs;
	};

}


#endif

