// mlp.h

#ifndef __NE_RTP_PHYS_MLP_CONTROLLER_H
#define __NE_RTP_PHYS_MLP_CONTROLLER_H

#include "../rtp_phys_controller.h"
#include "../rtp_phys_system.h"
#include "../../../params/enum_par.h"
#include "../../../rtp_mlp_controller.h"


namespace rtp_phys
{
	class mlp_controller:
		public i_phys_controller,
		public generic_mlp_controller
	{
	public:
		enum Type {
			TestType,
			TestQuadruped,
			TestBiped,
			Spaceship,

			Count,
			None = Count,
			Default = TestType,
		};

		static std::string const Names[Count];

		class enum_param_type: public rtp_enum_param_type
		{
		public:
			enum_param_type();
		};

		class param_type: public rtp_paramlist_param_type
		{
		public:
			virtual size_t provide_num_child_params(rtp_param_manager* mgr) const;
			virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const;
		};

		static std::pair< i_genome_mapping*, i_agent_factory* > create_instance_evolvable(rtp_param param);

		static YAML::Node get_schema(YAML::Node const& param_vals);
		static std::pair< i_genome_mapping*, i_agent_factory* > create_instance_evolvable(YAML::Node const& param);

	public:
		//typedef typename sat_system< dim >::decision decision_t;
		typedef phys_system::state state_t;
		typedef phys_system::scenario_data scenario_data_t;

		mlp_controller(size_t num_layers, size_t per_hidden, size_t num_outputs);

	public:
		//virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);
		virtual std::vector< double > map_nn_inputs(state_t const& st) = 0;

	protected:
		size_t const num_nn_layers;
		size_t const num_per_hidden;
		size_t const num_nn_outputs;
	};

	// TODO: Really need to have a generic mapper so that can choose subset of state values at runtime which will be mapped.
	// That said, obviously each subset needs to be identified still since an agent trained on one cannot be used with another.

	class mlp_test_controller: public mlp_controller
	{
	public:
//		typedef typename sat_system< dim >::decision decision_t;
		typedef phys_system::state state_t;
		typedef phys_system::scenario_data scenario_data_t;

		static size_t const NumNNInputs =
			//num_components< typename dim_traits_t::angular_velocity_t >::val;
			2;	// TODO:

	public:
		mlp_test_controller(size_t num_layers, size_t per_hidden, size_t num_outputs);
		virtual std::vector< double > map_nn_inputs(state_t const& st);

		virtual void update(phys_system::state& st, phys_system::scenario_data sdata);
	};

	class mlp_test_quadruped_controller: public mlp_controller
	{
	public:
		//		typedef typename sat_system< dim >::decision decision_t;
		typedef phys_system::state state_t;
		typedef phys_system::scenario_data scenario_data_t;

		static size_t const NumNNInputs = 19;

	public:
		mlp_test_quadruped_controller(size_t num_layers, size_t per_hidden, size_t num_outputs);
		virtual std::vector< double > map_nn_inputs(state_t const& st);

		virtual void update(phys_system::state& st, phys_system::scenario_data sdata);
	};

	class mlp_test_biped_controller: public mlp_controller
	{
	public:
		//		typedef typename sat_system< dim >::decision decision_t;
		typedef phys_system::state state_t;
		typedef phys_system::scenario_data scenario_data_t;

		static size_t const NumNNInputs = 10;

	public:
		mlp_test_biped_controller(size_t num_layers, size_t per_hidden, size_t num_outputs);
		virtual std::vector< double > map_nn_inputs(state_t const& st);

		virtual void update(phys_system::state& st, phys_system::scenario_data sdata);
	};

	class mlp_spaceship_controller: public mlp_controller
	{
	public:
		typedef phys_system::state state_t;
		typedef phys_system::scenario_data scenario_data_t;

	public:
		mlp_spaceship_controller(size_t num_layers, size_t per_hidden, size_t num_outputs);

		void create_nn(size_t num_inputs);
		
		virtual void update(phys_system::state& st, phys_system::scenario_data sdata);
	};

	class mlp_all_inputs_spaceship_controller: public mlp_spaceship_controller
	{
	public:
		static size_t const NumNNInputs;

		mlp_all_inputs_spaceship_controller(size_t num_layers, size_t per_hidden, size_t num_outputs);

	public:
		virtual std::vector< double > map_nn_inputs(state_t const& st);
	};


	class mlp_controller_factory: public i_agent_factory
	{
	public:
		mlp_controller_factory(mlp_controller::Type type, size_t num_layers, size_t num_per_hidden, size_t num_outputs);

	public:
		virtual i_agent* create();

	private:
		mlp_controller::Type m_type;
		size_t const m_num_nn_layers;
		size_t const m_num_per_hidden;
		size_t const m_num_nn_outputs;
	};

}


#endif

