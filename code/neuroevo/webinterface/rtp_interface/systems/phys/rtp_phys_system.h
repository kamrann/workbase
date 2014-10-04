// rtp_phys_system.h

#ifndef __NE_RTP_PHYS_SYSTEM_H
#define __NE_RTP_PHYS_SYSTEM_H

#include "../rtp_system.h"
#include "../rtp_decision.h"
#include "../rtp_agent.h"
#include "../../rtp_pop_wide_observer.h"
#include "rtp_phys_contacts.h"

#include "wt_param_widgets/param_accessor.h"

#include <Box2D/Box2D.h>

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include <set>
#include <chrono>


class b2World;

namespace rtp {

	// TODO: Better to store in more accessible form (ie. pre extract the values), or is that going to be
	// less maintainable? (one extra place to change when we add/modify a property of any agent type)
	struct phys_agent_specification
	{
		prm::param_accessor spec_acc;
		prm::param_accessor inst_acc;
	};


	class phys_scenario;
	class agent_body_spec;
	class agent_body;
	class i_controller;

	struct phys_sensor_defn;
	struct phys_sensor;

	struct entity_data;
	struct entity_fix_data;
	struct entity_joint_data;

	class phys_system:
		public i_system,
		public b2ContactListener
	{
	public:
		struct agent_state
		{
//			boost::shared_ptr< agent_body > body;

			agent_state()
			{

			}
		};

		struct envt_state
		{
			double						time;
//			std::unique_ptr< b2World >	world;

			envt_state(): time(0.0)//, world(nullptr)
			{}
		};

		struct state:
			public envt_state,
			public agent_state	// For now, just a single agent system
		{};

		// TODO: serializable state
		struct serializable_initial_state
		{
			b2Vec2 body_pos;
			float body_angle;
			b2Vec2 body_lin_vel;
			float body_ang_vel;
			boost::any scenario;

			serializable_initial_state():
				body_pos(0.0f, 0.0f),
				body_angle(0.0f),
				body_lin_vel(0.0f, 0.0f),
				body_ang_vel(0.0f)
			{}
		};

		struct trial_data
		{
			// TODO: May want to define a structure to represent rigid body state only (pos, orientation, velocities)
			// Then initial/final states would be a list of these for each body. Currently, using a pointer makes no sense.
			state initial_st;
			state final_st;
		};

		typedef boost::any scenario_data;

	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable);
		static std::string update_agentspeclist_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable);
		static std::string update_agentinstancelist_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable);
//		static std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > create_instance(prm::param_accessor param, bool evolvable = false);
//		static std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > create_instance(prm::param& param, bool evolvable = false);

		static std::unique_ptr< i_system_factory > generate_factory(prm::param_accessor param_vals);
		static agents_data generate_agents_data(prm::param_accessor param_vals, bool evolvable);

		static std::vector< std::string > get_state_values(prm::param_accessor param_vals);
		static std::vector< std::string > get_agent_type_names();
		static std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals);
		static size_t get_agent_num_effectors(prm::param agent_type);

	public:
		phys_system(std::unique_ptr< phys_scenario > scenario, /*agent_body_spec* spec,*/ double hertz);
		~phys_system();

		void release_body_user_data();

	public:
		virtual bool is_instantaneous() const override;
		virtual update_info get_update_info() const override;

//		virtual boost::any generate_initial_state() const;
//		virtual void set_state(boost::any const& st);
		virtual void initialize() override;
		virtual void clear_agents();
		virtual boost::optional< agent_id_t > register_agent(boost::shared_ptr< i_controller > agent);
		virtual agent_id register_agent(agent_specification const& spec);// , std::unique_ptr< i_controller >&& controller);
		void register_agent_controller(agent_id agent, std::unique_ptr< i_controller > controller);
		
		virtual i_agent const& get_agent(agent_id id) const;
		virtual i_controller const& get_agent_controller(agent_id id) const;

		virtual bool update(i_observer* obs);
		virtual void register_interactive_input(interactive_input const& input);
		virtual i_observer::observations_t record_observations(i_observer* obs) const;
		virtual std::shared_ptr< i_properties const > get_state_properties() const;
		virtual std::shared_ptr< i_property_values const > get_state_property_values() const;
		virtual double get_state_value(state_value_id id) const override;
		virtual std::unique_ptr< i_system_drawer > get_drawer() const;

	public:
		b2World* get_world()
		{
			return //m_state.world.get();
				m_world.get();
		}

		struct agent_instance
		{
			std::unique_ptr< agent_body >		agent;
			std::unique_ptr< i_controller >		controller;

			agent_instance();
			agent_instance(std::unique_ptr< agent_body > a, std::unique_ptr< i_controller > c);
			agent_instance(agent_instance&& rhs);
			agent_instance& operator= (agent_instance&& rhs);
			~agent_instance();
		};

		typedef std::vector< agent_instance > agent_list;
		typedef std::pair< agent_list::iterator, agent_list::iterator > agent_range;
		typedef std::pair< agent_list::const_iterator, agent_list::const_iterator > const_agent_range;

//		typedef std::vector< std::unique_ptr< i_controller > > controller_list;

		/* TODO: WTF why is use of the following causing internal compiler error???
		 - in phys_sys_drawer...
		agent_range get_agents()
		{
			return{ begin(m_agents), end(m_agents) };
		}

		const_agent_range get_agent_range() const
		{
			return{ begin(m_agents), end(m_agents) };
		}
		*/
	public:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

		void on_contact(b2Contact* contact, ContactType ctype);

	protected:
		state const& get_state() const
		{
			return m_state;
		}

		std::unique_ptr< agent_body > create_agent_from_specification(phys_agent_specification const& spec);

	private:
		std::unique_ptr< phys_scenario > m_scenario;
		state m_state;
		std::unique_ptr< b2World > m_world;
//		std::list< entity_data > m_entity_data;
//		std::list< entity_fix_data > m_fixture_data;
//		std::list< entity_joint_data > m_joint_data;
		std::unique_ptr< b2DestructionListener > m_world_destructor;

		trial_data m_td;
		double m_hertz;

	public:	// TODO: Temp public, see above re internal compiler error
		agent_list m_agents;

//#ifdef PERF_LOGGING
	public:
		std::chrono::high_resolution_clock::duration
			m_agent_update_time,
			m_b2d_update_time,
			m_observer_update_time;

		virtual void concatenate_performance_data(perf_data_t& pd) const override;
//#endif

		friend class phys_system_drawer;
	};

}


#endif

