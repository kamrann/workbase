// rtp_sat_system.h

#ifndef __NE_RTP_SAT_SYSTEM_H
#define __NE_RTP_SAT_SYSTEM_H

#include "../rtp_system.h"
#include "../rtp_decision.h"

#include "thrusters/ship_state.h"
#include "thrusters/thruster.h"


namespace rtp_sat {

	class sat_system_base: public i_system
	{
	public:
		class decision:
			public i_decision,
			public thruster_base::thruster_activation
		{
			decision(thruster_base::thruster_activation ta = thruster_base::thruster_activation()):
				i_decision(),
				thruster_base::thruster_activation(ta)
			{}
		};

	public:
		//virtual boost::optional< agent_id_t > register_agent();
		//virtual bool pending_decision(agent_id_t id);
		//virtual void register_solution_decision(i_decision const& dec);

	protected:
		//boost::optional< agent_id_t > m_agent;
		//boost::optional< decision > m_decision;

	public:
		static i_system* create_instance(rtp_param param);
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

		typedef boost::any scenario_data;

		class i_agent
		{
		public:
			virtual decision make_decision(state const& st, scenario_data sdata) = 0;
		};

	public:
		static rtp_named_param_list params();

		sat_system(rtp_param param);

	public:
		boost::optional< agent_id_t > register_agent(i_agent* agent);
		
		virtual void generate_initial_state(rgen_t& rgen);
		virtual bool update();
		virtual i_system_drawer* get_drawer() const;

	protected:
		state const& get_state() const
		{
			return m_state;
		}

	private:
		sat_scenario< dim >* m_scenario;
		boost::optional< i_agent* > m_agent;
		state m_state;

	template < WorldDimensionality >
	friend class sat_system_drawer;
	};

}

/*
template < WorldDimensionality dim >
class sat_system
{
public:
	class state
	{
	public:
		virtual ship_state< dim >& get_ship_state(size_t idx) = 0;
		virtual thruster_system< dim >& get_ship_thruster_sys(size_t idx) = 0;
		virtual double get_time() = 0;

		virtual ~state() {}
	};

	template < typename Scenario >
	class scen_state: public state
	{
	public:
		typedef Scenario scenario_t;
		typedef typename scenario_t::state state_t;

	public:
		scen_state(state_t const& _st): m_state(_st)
		{}

	public:
		virtual ship_state< dim >&//scenario_t::Dim >&
			get_ship_state(size_t idx)
		{
			return m_state.agents[idx];
		}

		virtual thruster_system< dim >& get_ship_thruster_sys(size_t idx)
		{
			return m_state.agents[idx].thruster_sys;
		}

		virtual double get_time()
		{
			return m_state.time;
		}

	private:
		state_t m_state;
	};
};
*/

#endif

