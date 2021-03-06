// rtp_sat_system.h

#ifndef __NE_RTP_SAT_SYSTEM_H
#define __NE_RTP_SAT_SYSTEM_H

#include "../rtp_system.h"
#include "../rtp_decision.h"
#include "../rtp_agent.h"
#include "../../params/paramlist_par.h"

#include "thrusters/ship_state.h"
#include "thrusters/thruster.h"


class rtp_stored_property_values;

namespace rtp_sat {

	class sat_system_base: public i_system
	{
	public:
		class decision:
			public i_decision,
			public thruster_base::thruster_activation
		{
		public:
			decision(thruster_base::thruster_activation ta = thruster_base::thruster_activation()):
				i_decision(),
				thruster_base::thruster_activation(ta)
			{}
		};

	public:
		//static i_system* create_instance(rtp_param param);
	};


	template < WorldDimensionality >
	class sat_scenario;

	template < WorldDimensionality dim >
	class sat_system: public sat_system_base
	{
	public:
		static WorldDimensionality const Dim = dim;

		typedef DimensionalityTraits< Dim > dim_traits_t;

		struct agent_state
		{
			ship_state< Dim >			ship;
			thruster_system< Dim >		thrusters;

			agent_state(): thrusters(nullptr)
			{

			}
		};

		struct envt_state
		{
			double	time;

			envt_state(): time(0.0)
			{}
		};

		struct state:
			public envt_state,
			public agent_state	// For now, just a single agent system
		{};

		enum StateValue {
			Time,
			PosX,
			PosY,
			// Z ?
			Angle,	// TODO: This is also Dimensions specific...
			VelX,
			VelY,
			Speed,
			AngularSpeed,
			KE,

			Count,
		};

		static std::string const StateValueNames[StateValue::Count];

		struct trial_data
		{
			state initial_st;
			state final_st;
		};

		typedef boost::any scenario_data;

		class i_sat_agent;

		class ship_config
		{
		public:
			enum Type {
				SquareMinimal,
				SquareComplete,

				Count,
			};

			static std::string const Names[Type::Count];

			class enum_param_type: public rtp_enum_param_type
			{
			public:
				enum_param_type();
			};

			static thruster_config< dim > create_instance(Type type, rtp_param param);
		};

		class i_sat_agent: public i_agent
		{
		public:
			enum Type {
				Passive,
				Interactive,

				Count,
			};

			static std::string const Names[Type::Count];

			class enum_param_type: public rtp_enum_param_type
			{
			public:
				enum_param_type();
			};

			static rtp_param_type* params(Type type);
			static i_sat_agent* create_instance(Type type, rtp_param param);

		public:
			virtual decision make_decision(state const& st, scenario_data sdata) = 0;
		};

		class evolvable_agent
		{
		public:
			enum Type {
				MLP,

				Count,
			};

			static std::string const Names[Type::Count];

			class enum_param_type: public rtp_enum_param_type
			{
			public:
				enum_param_type();
			};

			static rtp_param_type* params(Type ea_type);
			static std::tuple< i_genome_mapping*, i_agent_factory* > create_instance(Type type, rtp_param param, thruster_config< dim > const& cfg);
		};

		class agent_objective
		{
		public:
			enum Type {
				ReduceSpeed,
				MinAvgSpeed,
				ReduceAngularSpeed,
				MinAvgAngularSpeed,
				ReduceKinetic,
				MinAvgKinetic,
//				MinFuel,

				Count,
			};

			static std::string const Names[Count];

			class enum_param_type: public rtp_enum_param_type
			{
			public:
				enum_param_type();
			};

			static rtp_param_type* params(Type type);
			static i_observer* create_instance(Type type, rtp_param param);
		};

		class param_type: public rtp_paramlist_param_type
		{
		public:
			param_type(bool evolvable = false): m_evolvable(evolvable)
			{}

		public:
			virtual size_t provide_num_child_params(rtp_param_manager* mgr) const;
			virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const;

		private:
			bool m_evolvable;
		};

	public:
		static rtp_param_type* params(bool evolvable);
		static std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* > create_instance(rtp_param param, bool evolvable);

	private:
		sat_system(sat_scenario< dim >* scenario, thruster_config< dim > const& t_cfg);// rtp_param param, bool evolvable);

	public:
		virtual boost::any generate_initial_state(rgen_t& rgen) const;
		virtual void set_state(boost::any const& st);
		virtual void clear_agents();
		virtual boost::optional< agent_id_t > register_agent(boost::shared_ptr< i_agent > agent);
		virtual void register_interactive_input(interactive_input const& input);
		virtual bool update(i_observer* obs);
		virtual i_observer::observations_t record_observations(i_observer* obs) const;
		virtual boost::shared_ptr< i_properties const > get_state_properties() const;
		virtual boost::shared_ptr< i_property_values const > get_state_property_values() const;
		virtual i_system_drawer* get_drawer() const;

	protected:
		state const& get_state() const
		{
			return m_state;
		}

		static void set_state_prop(rtp_stored_property_values* pv, StateValue sv, boost::any v);

	private:
		sat_scenario< dim >* m_scenario;
		thruster_config< dim > m_ship_cfg;
		boost::optional< boost::shared_ptr< i_sat_agent > > m_agent;
		state m_state;
		trial_data m_td;

	template < WorldDimensionality >
	friend class sat_system_drawer;
	};

}


#endif

