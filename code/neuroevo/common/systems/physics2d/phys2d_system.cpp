// phys2d_system.cpp

#include "phys2d_system.h"
#include "phys2d_system_defn.h"
#include "phys2d_entity_data.h"
#include "phys2d_sensors.h"
#include "phys2d_systemstatevalues.h"
#include "scenario.h"

#include "phys2d_system_drawer.h"

#include "system_sim/basic_agent.h"
#include "system_sim/interactive_controller.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"

#include "b2d_util.h"


namespace sys {
	namespace phys2d {

		phys2d_system::agent_instance::agent_instance()
		{}

		phys2d_system::agent_instance::agent_instance(std::unique_ptr< i_agent > a, std::unique_ptr< i_controller > c):
			agent(std::move(a)), controller(std::move(c))
		{}

		phys2d_system::agent_instance::agent_instance(agent_instance&& rhs)
		{
			agent = std::move(rhs.agent);
			controller = std::move(rhs.controller);
		}

		phys2d_system::agent_instance& phys2d_system::agent_instance::operator= (agent_instance&& rhs)
		{
			agent = std::move(rhs.agent);
			controller = std::move(rhs.controller);
			return *this;
		}

		phys2d_system::agent_instance::~agent_instance()
		{}



		phys2d_system::phys2d_system(std::unique_ptr< scenario > scen, double hertz):
			m_scenario(std::move(scen))
		{
			m_hertz = hertz;
		}

		phys2d_system::~phys2d_system()
		{
			release_body_user_data();
		}

		void phys2d_system::release_body_user_data()
		{
			if(m_world)
			{
				for(auto body = m_world->GetBodyList(); body != nullptr; body = body->GetNext())
				{
					auto ud = body->GetUserData();
					if(ud)
					{
						delete static_cast<entity_data*>(ud);
						body->SetUserData(nullptr);
					}
				}
			}
		}


/*		i_system::update_info phys2d_system::get_update_info() const
		{
			update_info uinf;
			uinf.type = update_info::Type::Realtime;
			uinf.frequency = m_hertz;
			return uinf;
		}
*/

		class phys_world_destruction_listener: public b2DestructionListener
		{
			void SayGoodbye(b2Fixture* fix) override
			{
				auto ud = fix->GetUserData();
				if(ud)
				{
					delete static_cast<entity_fix_data*>(ud);
					fix->SetUserData(nullptr);
				}
			}

			void SayGoodbye(b2Joint* joint) override
			{
				auto ud = joint->GetUserData();
				if(ud)
				{
					delete static_cast<entity_joint_data*>(ud);
					joint->SetUserData(nullptr);
				}
			}
		};

		bool phys2d_system::initialize()
		{
			if(!basic_system::initialize())
			{
				return false;
			}

			m_world = std::make_unique< b2World >(b2Vec2(0.0f, 0.0f));
			m_world->SetContactListener(this);

			m_world_destructor = std::make_unique< phys_world_destruction_listener >();
			m_world->SetDestructionListener(m_world_destructor.get());

			if(!m_scenario->create(this))
			{
				return false;
			}

			for(auto& ag : m_agents)
			{
				// Or would it generally be better to just pass the already constructed system to agent_defn::create_agent()...?
				static_cast< basic_agent* >(ag.agent.get())->create(this);
			}

			// TODO: ??
			//auto scenario_init = m_scenario->generate_initial_state(m_rgen);
			//m_scenario->load_initial_state(scenario_init, m_world.get());

			m_time = 0.0;

			return true;
		}

		size_t phys2d_system::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors)
		{
			auto bound_id = accessors.size();
			auto initial_count = bindings.size();

			bindings[state_value_id{ phys2d_system_defn::s_state_values.left.at(phys2d_system_defn::StateValue::Time) }] = bound_id++;
			accessors.push_back([this]{ return m_time; });

			m_scenario->initialize_state_value_bindings(bindings, accessors);

			// TODO: or leave to basic_system???
			for(auto& ag : m_agents)
			{
				auto basic_ag = static_cast<basic_agent*>(ag.agent.get());
				basic_ag->initialize_state_value_bindings(bindings, accessors);
			}

			return bindings.size() - initial_count;
		}

		void phys2d_system::clear_agents()
		{
#if 0
			m_agent = boost::none;
#endif
		}


		/*

			agent->activate_effectors(std::vector< double >(agent->get_num_effectors(), 0.0));

	*/

		i_system::agent_id phys2d_system::register_agent(agent_ptr agent)
		{
//			// Or would it generally be better to just pass the already constructed system to agent_defn::create_agent()...?
//			static_cast< basic_agent* >(agent.get())->create(this);

			i_system::agent_id id = m_agents.size();
			//auto agent = create_agent_from_specification(boost::any_cast<phys_agent_specification const&>(spec));
			m_agents.emplace_back(agent_instance{ std::move(agent), nullptr });

			return id;
		}

		void phys2d_system::register_agent_controller(agent_id agent, controller_ptr controller)
		{
			m_agents[agent].controller = std::move(controller);
		}

		i_agent const& phys2d_system::get_agent(agent_id id) const
		{
			return *m_agents[id].agent;
		}

		i_controller const& phys2d_system::get_agent_controller(agent_id id) const
		{
			return *m_agents[id].controller;
		}


#if 0
		boost::any phys2d_system::generate_initial_state() const
		{
			// TODO: Currently working on assumption that this does not depend on which controllers are registered, so that can be called
			// once before start of trial, and used for any subsequently registered controller

			serializable_initial_state init_st;

			init_st.scenario = m_scenario->generate_initial_state(m_rgen);
#if 0
			init_st.body_pos.x = m_body_spec->m_initial_pos_x.get(rgen);
			init_st.body_pos.y = m_body_spec->m_initial_pos_y.get(rgen);
			init_st.body_angle = m_body_spec->m_initial_orientation.get(rgen) * b2_pi / 180;
			float motion_angle = m_body_spec->m_initial_linear_dir.get(rgen) * b2_pi / 180;
			float lin_speed = m_body_spec->m_initial_linear_speed.get(rgen);
			init_st.body_lin_vel.Set(
				-std::sin(motion_angle) * lin_speed,
				std::cos(motion_angle) * lin_speed
				);
			init_st.body_ang_vel = m_body_spec->m_initial_ang_vel.get(rgen) * b2_pi / 180;
#endif	
			return boost::any(init_st);
		}
#endif

#if 0
		void phys2d_system::set_state(boost::any const& st)
		{
			serializable_initial_state const& init_st = boost::any_cast<serializable_initial_state const&>(st);

			m_state = state();
			m_state.world.reset(new b2World(b2Vec2(0.0f, 0.0f)));

			m_state.world->SetContactListener(this);

			m_scenario->load_initial_state(init_st.scenario, m_state);
#if 0
			m_state.body.reset(m_body_spec->create_body(m_state.world.get()));
			m_state.body->activate_effectors(std::vector< double >(m_body_spec->get_num_effectors(), 0.0));

			m_state.body->rotate(init_st.body_angle);
			m_state.body->translate(init_st.body_pos);
			m_state.body->set_linear_velocity(init_st.body_lin_vel);
			m_state.body->set_angular_velocity(init_st.body_ang_vel);
#endif
			// TODO: For now, step forwards a couple of seconds so that sim starts with bodies already at equilibrium
			/*		for(size_t i = 0; i < 100; ++i)
					{
					m_state.world->Step(1.0f / 50, 8, 3);
					}
					*/
			// TODO: If we want to use this method to set a general state, this shouldn't be here...
			m_td = trial_data();
			m_td.initial_st = m_state;
		}
#endif

		void phys2d_system::register_interactive_input(interactive_input const& input)
		{
			// TODO: Need to either pass in boolean is_interactive argument to register_agent_controller(),
			// or provide a method i_controller::is_interactive()

			for(auto& entry : m_agents)
			{
				// Temp hack, see above
				auto as_interactive = dynamic_cast<i_interactive_controller*>(entry.controller.get());
				if(as_interactive != nullptr)
				{
					as_interactive->register_input(input);
				}
			}
		}

		bool phys2d_system::update(i_observer* obs)
		{
			float const timestep = 1.0f / m_hertz;

			/*
			This should generalise to something like:

			sys_update_data sud
			for each agent a:							// a is agent of unknown type
			register_agent_actions(a(state), sud)	// This uses virtual mechanism to determine what effect agent a's actions will have on system

			process_system_updates(sud)					// Update system taking into account all agents' actions simultaneously

			This way, we can have different types of agents, which can affect the system in different ways.
			We no longer have a single decision type, but rather let agent type specific code register whatever effects it needs on the system.
			*/

			auto start_ = std::chrono::high_resolution_clock::now();

			for(size_t idx = 0; idx < m_agents.size(); ++idx)
			{
				auto& agent = *m_agents[idx].agent;
				auto& controller = *m_agents[idx].controller;

				auto required_inputs = controller.get_input_ids();
				auto num_inputs = required_inputs.size();
				i_controller::input_list_t inputs;
				for(auto const& input_id : required_inputs)
				{
					inputs.push_back(agent.get_sensor_value(input_id));
				}

				auto outputs = controller.process(inputs);
				// TODO: hack for passive controller.
				// ideally effector bindings would deal with this
				if(outputs.empty())
				{
					continue;
				}
				//
				agent.activate_effectors(outputs);
			}

			auto end_ = std::chrono::high_resolution_clock::now();
			m_agent_update_time += end_ - start_;

			start_ = std::chrono::high_resolution_clock::now();

			m_world->Step(timestep, 4, 2);	// TODO: iterations?

			end_ = std::chrono::high_resolution_clock::now();
			m_b2d_update_time += end_ - start_;

			m_time += timestep;

			if(obs)
			{
				auto start_ = std::chrono::high_resolution_clock::now();

				//i_phys_observer* phys_obs = (i_phys_observer*)obs;
				obs->update(this);

				auto end_ = std::chrono::high_resolution_clock::now();
				m_observer_update_time += end_ - start_;
			}

			return !m_scenario->is_complete()
				&& m_time < 10.0;	// TODO: Temp
		}

		void phys2d_system::on_contact(b2Contact* contact, ContactType ctype)
		{
#if 0
			auto fixA = contact->GetFixtureA();
			auto fixB = contact->GetFixtureB();

			auto efdA = get_fixture_data(fixA);
			auto efdB = get_fixture_data(fixB);

			if(efdA && efdA->type == entity_fix_data::Type::Sensor)
			{
				// TODO: generic sensor
				auto sen = boost::any_cast<contact_based_sensor*>(efdA->value);
				sen->on_contact(ctype);	// TODO: Probably want to pass entity data for sensed fixture
			}
			else if(efdB && efdB->type == entity_fix_data::Type::Sensor)
			{
				// TODO: generic sensor
				auto sen = boost::any_cast<contact_based_sensor*>(efdB->value);
				sen->on_contact(ctype);	// TODO: Probably want to pass entity data for sensed fixture
			}
			else
			{
				auto bodyA = fixA->GetBody();
				auto bodyB = fixB->GetBody();

				auto edA = get_body_data(bodyA);
				auto edB = get_body_data(bodyB);

				agent_body* agentA = nullptr;
				if(edA && edA->type == entity_data::Type::Agent)
				{
					agentA = boost::any_cast<agent_body*>(edA->type_value);
					agentA->on_contact(fixA, fixB, ctype);
				}

				if(edB && edB->type == entity_data::Type::Agent)
				{
					auto agentB = boost::any_cast<agent_body*>(edB->type_value);
					bool same_entity = agentB == agentA;
					if(!same_entity)
					{
						agentB->on_contact(fixB, fixA, ctype);
					}
				}
			}
#endif
		}

		void phys2d_system::BeginContact(b2Contact* contact)
		{
			on_contact(contact, ContactType::Begin);
		}

		void phys2d_system::EndContact(b2Contact* contact)
		{
			on_contact(contact, ContactType::End);
		}

		void phys2d_system::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
		{
#if 0
			auto fixA = contact->GetFixtureA();
			auto fixB = contact->GetFixtureB();

			auto efdA = get_fixture_data(fixA);
			auto efdB = get_fixture_data(fixB);

			auto bodyA = fixA->GetBody();
			auto bodyB = fixB->GetBody();

			auto edA = get_body_data(bodyA);
			auto edB = get_body_data(bodyB);

			auto A_is_agent = edA && edA->type == entity_data::Type::Agent;
			auto B_is_agent = edB && edB->type == entity_data::Type::Agent;
			auto agent_involved = A_is_agent || B_is_agent;
			auto sensor_involved = fixA->IsSensor() || fixB->IsSensor();
			if(sensor_involved || !agent_involved)
			{
				return;
			}

			/*
			Each manifold has 0, 1 or 2 points.
			Still not 100% on this, but essentially it's looking at individual collisions, of which there can be
			more than one in the duration of a single contact. A contact represents contact between 2 fixtures.
			A one fixture could rotate around causing another collision/contact point with the same other fixture,
			but this wouldn't result in BeginContact being called.

			BeginContact is always followed by one successful pass of the below if statement, however more passes
			may occur before EndContact is eventually invoked.
			*/
			b2WorldManifold worldManifold;
			contact->GetWorldManifold(&worldManifold);
			b2PointState state1[2], state2[2];
			b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
			if(state2[0] == b2_addState)
			{
				b2Vec2 point = worldManifold.points[0];

				b2Vec2 vA = bodyA->GetLinearVelocityFromWorldPoint(point);
				b2Vec2 vB = bodyB->GetLinearVelocityFromWorldPoint(point);
				auto vRel = vA - vB;

				auto approach_speed = b2Dot(vRel, worldManifold.normal);
				if(approach_speed < 0)
				{
					// TODO: Check this.
					// Assuming normal is always from A->B.
					// Sometimes negative, though generally not. What's the deal?
					return;
				}

				agent_body* agentA = nullptr;
				if(A_is_agent)
				{
					agentA = boost::any_cast<agent_body*>(edA->type_value);
					agentA->on_collision(fixA, fixB, approach_speed);
				}

				if(B_is_agent)
				{
					auto agentB = boost::any_cast<agent_body*>(edB->type_value);
					bool same_entity = agentB == agentA;
					if(!same_entity)
					{
						agentB->on_collision(fixB, fixA, approach_speed);
					}
				}
			}
#endif
		}


		i_observer::observations_t phys2d_system::record_observations(i_observer* obs) const
		{
			return obs->record_observations(boost::none);//m_td);
		}

		system_drawer_ptr phys2d_system::get_drawer() const
		{
			return std::make_unique< phys2d_system_drawer >(*this);
		}

#if 0
		std::shared_ptr< i_properties const > phys2d_system::get_state_properties() const
		{
			auto props = std::make_shared< rtp_stored_properties >();
			for(size_t i = 0; i < StateValue::Count; ++i)
			{
				props->add_property(StateValueNames[i]);
			}

			// TODO: !!!!!!!!!!!!
			//props->add_property("Collision Count");
			//

			for(auto const& entry : m_agents)
			{
				auto const& agent = entry.agent;

				auto sensor_names = agent->get_sensors();
				//			size_t sensor_index = 0;
				for(auto const& sensor : sensor_names)
				{
					auto name = agent->get_name() + "_" + sensor;
					props->add_property(name);
					//				++sensor_index;
				}

				auto num_effectors = agent->get_num_effectors();
				for(size_t i = 0; i < num_effectors; ++i)
				{
					std::stringstream ss;
					ss << agent->get_name() + "_Effector " << (i + 1);
					props->add_property(ss.str());
				}
			}

			return props;
		}

		std::shared_ptr< i_property_values const > phys2d_system::get_state_property_values() const
		{
			auto vals = std::make_shared< rtp_stored_property_values >();

			set_state_prop(vals.get(), StateValue::Time, m_state.time);
			/*
					b2Vec2 pos = m_state.body->get_position();
					set_state_prop(vals, AgentPosX, pos.x);
					set_state_prop(vals, AgentPosY, pos.y);
					b2Vec2 vel = m_state.body->get_linear_velocity();
					set_state_prop(vals, AgentVelX, vel.x);
					set_state_prop(vals, AgentVelY, vel.y);
					set_state_prop(vals, AgentSpeed, vel.Length());
					//		set_state_prop(vals, AgentAngle, m_state.body->get_orientation());
					set_state_prop(vals, AgentKE, m_state.body->get_kinetic_energy());
					*/

			// TODO: Hack!!!!!!!!!!!!!!!!!
			//vals->set_value("Collision Count", m_scenario->get_state_value("Collision Count"));
			//

			for(auto const& entry : m_agents)
			{
				auto const& agent = entry.agent;

				auto sensors = agent->get_sensors();
				size_t sensor_index = 0;
				for(auto sensor_name : sensors)
				{
					sensor_name = agent->get_name() + "_" + sensor_name;
					vals->set_value(sensor_name, agent->get_sensor_value(sensor_index));
					++sensor_index;
				}

				auto num_effectors = agent->get_num_effectors();
				for(size_t e = 0; e < num_effectors; ++e)
				{
					std::stringstream ss;
					ss << agent->get_name() + "_Effector " << (e + 1);
					vals->set_value(ss.str(), agent->get_effector_activation(e));
				}
			}

			return vals;
		}

		double phys2d_system::get_state_value(state_value_id id) const
		{
			// TODO:
			if(id.compare("Time") == 0)
			{
				return m_state.time;
			}
			else
			{
				return m_scenario->get_state_value(id);
			}
		}
#endif

		void phys2d_system::concatenate_performance_data(perf_data_t& pd) const
		{
			pd["Agents"] += m_agent_update_time;
			pd["Box2D"] += m_b2d_update_time;
			pd["Observers"] += m_observer_update_time;
		}

}
}

