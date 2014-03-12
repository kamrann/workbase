// rtp_phys_system.h

#ifndef __NE_RTP_PHYS_SYSTEM_H
#define __NE_RTP_PHYS_SYSTEM_H

#include "../rtp_system.h"
#include "../rtp_decision.h"
#include "../rtp_agent.h"
#include "../../rtp_pop_wide_observer.h"
#include "../../params/paramlist_par.h"

#include <boost/shared_ptr.hpp>

#include <set>


class rtp_stored_property_values;

class b2World;

namespace rtp_phys {

	class phys_system_base: public i_system
	{
	public:
	};


	class phys_scenario;
	class agent_body_spec;
	class agent_body;
	class i_phys_controller;

	class phys_system: public phys_system_base
	{
	public:
		struct agent_state
		{
			boost::shared_ptr< agent_body > body;

			agent_state()
			{

			}
		};

		struct envt_state
		{
			double							time;
			boost::shared_ptr< b2World >	world;

			envt_state(): time(0.0), world(nullptr)
			{}
		};

		struct state:
			public envt_state,
			public agent_state	// For now, just a single agent system
		{};

		enum StateValue {
			Time,
			AgentPosX,
			AgentPosY,
			AgentAngle,

			Count,
		};

		static std::string const StateValueNames[StateValue::Count];

		struct trial_data
		{
			// TODO: May want to define a structure to represent rigid body state only (pos, orientation, velocities)
			// Then initial/final states would be a list of these for each body. Currently, using a pointer makes no sense.
			state initial_st;
			state final_st;
		};

		typedef boost::any scenario_data;


		class param_type: public rtp_paramlist_param_type
		{
		public:
			param_type(bool evolvable = false);

		public:
			virtual size_t provide_num_child_params(rtp_param_manager* mgr) const;
			virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const;

		private:
			bool m_evolvable;
		};

	public:
		static rtp_param_type* params(bool evolvable);
		static std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > create_instance(rtp_param param, bool evolvable);

	private:
		phys_system(phys_scenario* scenario, agent_body_spec* spec);

	public:
		virtual boost::any generate_initial_state(rgen_t& rgen) const;
		virtual void set_state(boost::any const& st);
		virtual void clear_agents();
		virtual boost::optional< agent_id_t > register_agent(boost::shared_ptr< i_agent > agent);
		virtual bool update(i_observer* obs);
		virtual void register_interactive_input(interactive_input const& input);
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
		phys_scenario* m_scenario;
		agent_body_spec* m_body_spec;
		boost::optional< boost::shared_ptr< i_phys_controller > > m_agent;
		state m_state;
		trial_data m_td;

	friend class phys_system_drawer;
	};

}


#endif

