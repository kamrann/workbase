// agents.h

#ifndef __NE_RTP_SAT_FULL_STOP_AGENTS_H
#define __NE_RTP_SAT_FULL_STOP_AGENTS_H

#include "../../rtp_sat_system.h"
#include "../../../rtp_interactive_agent.h"
#include "../../../../params/enum_par.h"
#include "../../../../rtp_mlp_controller.h"


namespace rtp_sat
{
	template < WorldDimensionality dim >
	class passive_agent: public sat_system< dim >::i_sat_agent
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

	public:
		virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);
	};

	template < WorldDimensionality dim >
	class interactive_agent:
		public sat_system< dim >::i_sat_agent,
		public i_interactive_agent
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

	public:
		virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);

		interactive_agent();
		virtual void register_input(interactive_input const& input);

	protected:
		std::vector< bool > inputs;
	};


	template < WorldDimensionality dim >
	class mlp_agent:
		public sat_system< dim >::i_sat_agent,
		public generic_mlp_controller
	{
	public:
		enum Type {
			AngVel,
			VelAngle,
			VelAnglePos,

			Count,
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

		//static rtp_named_param_list params(Type mlp_type);
		static std::tuple< i_genome_mapping*, i_agent_factory* > create_instance_evolvable(rtp_param param, thruster_config< dim > const& cfg);

	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

		mlp_agent(size_t num_layers, size_t num_outputs);

	public:
		virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);
		virtual std::vector< double > map_nn_inputs(state_t const& st) = 0;

	protected:
		size_t const num_nn_layers;
		size_t const num_nn_outputs;
	};

	// TODO: Really need to have a generic mapper so that can choose subset of state values at runtime which will be mapped.
	// That said, obviously each subset needs to be identified still since an agent trained on one cannot be used with another.

	template < WorldDimensionality dim >
	class mlp_ang_vel_agent: public mlp_agent< dim >
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

		typedef DimensionalityTraits< dim > dim_traits_t;
//		static size_t const NumNNLayers = 2;
		static size_t const NumNNInputs =
			num_components< typename dim_traits_t::angular_velocity_t >::val;

	public:
		mlp_ang_vel_agent(size_t num_layers, size_t num_outputs);
		virtual std::vector< double > map_nn_inputs(state_t const& st);
	};

	template < WorldDimensionality dim >
	class mlp_vel_and_angle_agent: public mlp_agent< dim >
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

		typedef DimensionalityTraits< dim > dim_traits_t;
//		static size_t const NumNNLayers = 3;
		static size_t const NumNNInputs =
			num_components< typename dim_traits_t::velocity_t >::val +
			num_components< typename dim_traits_t::angular_velocity_t >::val +
			num_components< typename dim_traits_t::orientation_t >::val;

	public:
		mlp_vel_and_angle_agent(size_t num_layers, size_t num_outputs);
		virtual std::vector< double > map_nn_inputs(state_t const& st);
	};

	template < WorldDimensionality dim >
	class mlp_vel_angle_pos_agent: public mlp_agent< dim >
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

		typedef DimensionalityTraits< dim > dim_traits_t;
		static size_t const NumNNInputs =
			num_components< typename dim_traits_t::velocity_t >::val +
			num_components< typename dim_traits_t::angular_velocity_t >::val +
			num_components< typename dim_traits_t::position_t >::val +
			num_components< typename dim_traits_t::orientation_t >::val;

	public:
		mlp_vel_angle_pos_agent(size_t num_layers, size_t num_outputs);
		virtual std::vector< double > map_nn_inputs(state_t const& st);
	};


	template < WorldDimensionality dim >
	class mlp_agent_factory: public i_agent_factory
	{
	public:
		mlp_agent_factory(typename mlp_agent< dim >::Type type, size_t num_layers, size_t num_outputs);

	public:
		virtual i_agent* create();

	private:
		typename mlp_agent< dim >::Type m_type;
		size_t const m_num_nn_layers;
		size_t const m_num_nn_outputs;
	};
}


#endif

