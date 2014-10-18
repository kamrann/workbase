// phys2d_system.h

#ifndef __NE_PHYS2D_SYSTEM_H
#define __NE_PHYS2D_SYSTEM_H

#include "system_sim/basic_system.h"

#include "phys2d_contacts.h"
#include "scenario.h"

#include "params/param_fwd.h"

#include <Box2D/Box2D.h>

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include <set>
#include <chrono>


class b2World;

namespace sys {
	namespace phys2d {

		class scenario;

		class phys2d_system:
			public basic_system,
			public b2ContactListener
		{
		public:
			phys2d_system(std::unique_ptr< scenario > scen, double hertz);
			~phys2d_system();

			void release_body_user_data();

		public:
//			virtual bool is_instantaneous() const override;
//			virtual update_info get_update_info() const override;

			virtual bool initialize() override;
			virtual void clear_agents() override;
			
			virtual agent_id register_agent(agent_ptr agent) override;
			virtual void register_agent_controller(agent_id agent, controller_ptr controller) override;

			virtual i_agent const& get_agent(agent_id id) const override;
			virtual i_controller const& get_agent_controller(agent_id id) const override;

			virtual void register_interactive_input(interactive_input const& input) override;
			virtual bool update(i_observer* obs) override;
			virtual i_observer::observations_t record_observations(i_observer* obs) const override;

			virtual system_drawer_ptr get_drawer() const override;

		protected:
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) override;

		public:
			double get_time() const
			{
				return m_time;
			}
			
			b2World* get_world() const
			{
				return m_world.get();
			}

			struct agent_instance
			{
				std::unique_ptr< i_agent >			agent;
				std::unique_ptr< i_controller >		controller;

				agent_instance();
				agent_instance(std::unique_ptr< i_agent > a, std::unique_ptr< i_controller > c);
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

		private:
			std::unique_ptr< scenario > m_scenario;
			std::unique_ptr< b2World > m_world;
			std::unique_ptr< b2DestructionListener > m_world_destructor;

			double m_hertz;
			double m_time;

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
		};

	}
}


#endif

