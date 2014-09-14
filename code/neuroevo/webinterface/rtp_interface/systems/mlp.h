// mlp.h

#ifndef __NE_RTP_MLP_CONTROLLER_H
#define __NE_RTP_MLP_CONTROLLER_H

#include "rtp_controller.h"
#include "rtp_agent.h"
#include "../rtp_mlp_controller.h"

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
		mlp_controller(agent_sensor_list const& _inputs, size_t num_layers, size_t per_hidden, size_t num_outputs);

	public:
		virtual input_id_list_t get_input_ids() const;
		virtual output_list_t process(input_list_t const& inputs);

//		virtual std::vector< double > map_nn_inputs(state_t const& st);
//		virtual void update(phys_system::state& st, phys_system::scenario_data sdata);

	protected:
		size_t const num_nn_layers;
		size_t const num_per_hidden;
		size_t const num_nn_outputs;

		agent_sensor_list inputs;
	};


	class mlp_controller_factory: public i_controller_factory
	{
	public:
		mlp_controller_factory(
//			agent_sensor_list const& inputs,
			std::vector< std::string > const& inputs,
			size_t num_layers, size_t num_per_hidden, size_t num_outputs);

	public:
		virtual std::unique_ptr< i_controller > create(i_agent const* attached_agent) const override;
		virtual bool is_interactive() const override
		{
			return false;
		}

	private:
		std::vector< std::string > const m_inputs;
		size_t const m_num_nn_layers;
		size_t const m_num_per_hidden;
		size_t const m_num_nn_outputs;
	};

}


#endif

