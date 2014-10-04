// rtp_phys_system.cpp

#include "rtp_phys_system.h"
#include "rtp_phys_entity_data.h"
#include "rtp_phys_sensors.h"
#include "../rtp_system_agents_data.h"
#include "rtp_phys_systemstatevalues.h"
#include "rtp_phys_scenario.h"
#include "rtp_phys_system_drawer.h"
#include "../../rtp_composite_observer.h"
#include "rtp_phys_objectives.h"
#include "../../rtp_manual_objective.h"
#include "../../rtp_resultant_objective.h"
#include "rtp_phys_agent_body_spec.h"
#include "rtp_phys_agent_body.h"
#include "rtp_phys_controller.h"
#include "../../rtp_evolvable_controller.h"
#include "../rtp_interactive_controller.h"
#include "../rtp_interactive_input.h"
#include "../../rtp_genome_mapping.h"
#include "../../rtp_properties.h"
#include "../../rtp_pop_wide_observer.h"
#include "../../rtp_single_objective.h"
#include "../../rtp_pareto.h"

#include "../phys/bodies/test_body.h"
#include "../phys/bodies/minimal_biped_body.h"
#include "../phys/bodies/test_biped_body.h"
#include "../phys/bodies/test_quadruped_body.h"
#include "../phys/bodies/basic_spaceship.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

#include "b2d_util.h"

#include <Wt/WComboBox>

#include <chrono>


namespace rtp {

	phys_system::agent_instance::agent_instance()
	{}

	phys_system::agent_instance::agent_instance(std::unique_ptr< agent_body > a, std::unique_ptr< i_controller > c):
		agent(std::move(a)), controller(std::move(c))
	{}

	phys_system::agent_instance::agent_instance(agent_instance&& rhs)
	{
		agent = std::move(rhs.agent);
		controller = std::move(rhs.controller);
	}

	phys_system::agent_instance& phys_system::agent_instance::operator= (agent_instance&& rhs)
	{
		agent = std::move(rhs.agent);
		controller = std::move(rhs.controller);
		return *this;
	}

	phys_system::agent_instance::~agent_instance()
	{}


	std::string const StateValueNames[] = {
		"Time",
	};

	void set_state_prop(rtp_stored_property_values* pv, StateValue sv, boost::any v)
	{
		pv->set_value(StateValueNames[sv], v);
	}

	namespace sb = prm::schema;

	std::vector< std::pair< prm::qualified_path, std::string > > get_unattached_evolved_instances(prm::param_accessor param_vals)
	{
		std::vector< std::pair< prm::qualified_path, std::string > > names;

		auto node = param_vals["agent_list"];
		auto agent_list_path = resolve_id("agent_list", param_vals.get_root(), param_vals.get_current_path(), param_vals.get_all_paths());
		for(auto agent_entry : node)
		{
			auto agent_repeat_num = agent_entry.first.as< unsigned int >();
			auto agent = agent_entry.second;

			auto rel_path = prm::qualified_path(std::string("spec")) + std::string("instance_list");
			auto instances_node = prm::find_value(agent, rel_path);

			// Commented as can potentially be empty map, which is treated as null node
			//			assert(!instances_node.IsNull());
			//			assert(instances_node.IsMap());
			auto inst_list_path = agent_list_path;
			inst_list_path.leaf().set_index(agent_repeat_num);
			inst_list_path += rel_path;

			for(auto inst_entry : instances_node)
			{
				auto inst_repeat_num = inst_entry.first.as< unsigned int >();
				auto inst = inst_entry.second;

				auto rel_path =
					prm::qualified_path(std::string("instance"))
					+ std::string("controller_params")
					+ std::string("controller_type");
				auto controller_type_node = prm::find_value(inst, rel_path);
				auto controller_type = controller_type_node[0].as< i_controller::Type >();

				if(controller_type != i_controller::Evolved)
				{
					continue;
				}

				rel_path = prm::qualified_path(std::string("instance")) + std::string("instance_name");
				auto name_node = prm::find_value(inst, rel_path);
				assert(name_node);

				auto inst_path = inst_list_path;
				inst_path.leaf().set_index(inst_repeat_num);

				names.emplace_back(std::make_pair(
					inst_path,
					name_node.as< std::string >()
					));
			}
		}

		return names;
	}

	std::string phys_system::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable)
	{
		auto relative = std::string{ "physics2d_params" };
		auto path = prefix + relative;

		(*provider)[path + std::string("updates_per_sec")] = [](prm::param_accessor)
		{
			auto s = sb::integer("updates_per_sec", 60, 10, 1000);
			sb::label(s, "Updates/Sec");
			return s;
		};

		auto scenario_rel = phys_scenario::update_schema_providor(provider, path);
		auto agentspeclist_rel = update_agentspeclist_schema_providor(provider, path, evolvable);
		auto agentinstancelist_rel = update_agentinstancelist_schema_providor(provider, path, evolvable);

		/*		std::string evolvablecontrollers_rel;
				if(evolvable)
				{
				evolvablecontrollers_rel = update_evolvablecontrollers_schema_providor(provider, path);
				}
				*/
		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("updates_per_sec"))(param_vals));
			sb::append(s, provider->at(path + scenario_rel)(param_vals));

			sb::append(s, provider->at(path + agentspeclist_rel)(param_vals));

			/*			if(evolvable)
						{
						sb::append(s, provider->at(path + evolvablecontrollers_rel)(param_vals));
						}
						*/
			sb::append(s, provider->at(path + agentinstancelist_rel)(param_vals));

			sb::border(s, std::string{ "Physics 2D" });
			sb::enable_import(s, "system.physics2d");
			return s;
		};

		return relative;
	}

	std::string phys_system::update_agentspeclist_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable)
	{
		auto relative = std::string{ "agent_spec_list" };
		auto path = prefix + relative;

		auto spec_rel = agent_body_spec::update_schema_providor(provider, path);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::repeating_list(relative, spec_rel, 0, 0);
			sb::label(s, "Agent Specificatons");
			sb::trigger(s, "agentspec_added_removed");
			sb::enable_import(s, "physics2d.agent_spec_list");
			return s;
		};

		return relative;
	}

	std::string phys_system::update_agentinstancelist_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable)
	{
		auto relative = std::string{ "agent_instance_list" };
		auto path = prefix + relative;

		auto instance_rel = agent_body::update_schema_providor(provider, path, evolvable);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::repeating_list(relative, instance_rel, 0, 0);
			// TODO:			sb::trigger(s, "agent_modified");
			sb::label(s, "Agent Instances");
			//sb::enable_import(s, "physics2d.agent_instance_list");
			return s;
		};

		return relative;
	}

	std::unique_ptr< i_system_factory > phys_system::generate_factory(prm::param_accessor param_vals)
	{
		// TODO: standardized way of testing whether subcomponent is fully specified
		// This is weird, just executing code to see if it throws or not
		phys_scenario::create_instance(param_vals);
		//

		return std::make_unique< system_factory >([=]()
		{
			auto acc(param_vals);
			acc.push_relative_path(prm::qualified_path("physics2d_params"));
			auto update_frequency = acc["updates_per_sec"].as< int >();
			auto scenario = phys_scenario::create_instance(param_vals);
			acc.pop_relative_path();
			return std::make_unique< phys_system >(std::move(scenario), update_frequency);
		});
	}

	agents_data phys_system::generate_agents_data(prm::param_accessor param_vals, bool evolvable)
	{
		agents_data data;
		size_t controller_id = 0;

		param_vals.push_relative_path(prm::qualified_path("physics2d_params"));

		std::map< std::string, prm::param_accessor > spec_map;

		auto spec_list_node = param_vals["agent_spec_list"];
		for(auto const& node : spec_list_node)
		{
			auto inst_num = node.first.as< unsigned int >();
			auto rel = prm::qualified_path("agent_spec_list");
			rel.leaf().set_index(inst_num);
			param_vals.push_relative_path(rel);

			//auto path = param_vals.get_current_path() + std::string{ "spec" };
			//auto key = path.to_string();
			auto key = param_vals["name"].as< std::string >();
			spec_map[key] = param_vals;

			param_vals.pop_relative_path();
		}

		//std::map< prm::qualified_path, size_t > evolved_map;
		std::map< std::string, size_t > evolved_map;
		if(evolvable)
		{
			auto evolved_path = param_vals.find_path("evolved_container");
			auto ev_acc = param_vals.from_absolute(evolved_path);
			auto evolvable_list_node = ev_acc["evolved_list"];
			for(auto const& node : evolvable_list_node)
			{
				auto inst_num = node.first.as< unsigned int >();
				auto rel = prm::qualified_path{ "evolved_list" };
				rel.leaf().set_index(inst_num);
				ev_acc.push_relative_path(rel);
				ev_acc.push_relative_path(std::string{ "evolved" });

				std::unique_ptr< i_controller_factory > controller_factory;
				std::unique_ptr< i_genome_mapping > mapping;
				std::tie(mapping, controller_factory) = evolvable_controller::create_instance(ev_acc);

				std::vector< std::unique_ptr< i_observer > > required_observations;
				std::unique_ptr< i_population_wide_observer > resultant_obj = resultant_objective::create_instance(ev_acc, required_observations);
				std::unique_ptr< i_observer > observer = composite_observer::create_instance(std::move(required_observations));
				data.controller_factories[controller_id] = std::move(controller_factory);
				data.evolved[controller_id].genome_mapping = std::move(mapping);
				data.evolved[controller_id].resultant_obj = std::move(resultant_obj);
				data.evolved[controller_id].observer = std::move(observer);

				//evolved_map[param_vals.get_current_path()] = controller_id;
				evolved_map[ev_acc["name"].as< std::string >()] = controller_id;
				controller_id++;

				ev_acc.pop_relative_path();
				ev_acc.pop_relative_path();
			}
		}

		auto agent_list_node = param_vals["agent_instance_list"];
		for(auto const& node : agent_list_node)
		{
			auto inst_num = node.first.as< unsigned int >();
			auto rel = prm::qualified_path("agent_instance_list");
			rel.leaf().set_index(inst_num);
			param_vals.push_relative_path(rel);

			auto name = param_vals["spec_reference"][0].as< std::string >();
			auto spec_accessor = spec_map[name];

			agents_data::agent_instance_data inst_data;
			inst_data.spec = phys_agent_specification{ spec_accessor, param_vals };

			auto controller = i_controller::create_factory(param_vals);

			if(evolvable && controller == nullptr)
			{
				param_vals.push_relative_path(std::string{ "agent" });
				param_vals.push_relative_path(std::string{ "controller_params" });
				param_vals.push_relative_path(std::string{ "controller_type_specific_params" });

				auto evolved_controller_key = param_vals["evolved_controller_sel"][0].as< std::string >();
				inst_data.controller_id = evolved_map[evolved_controller_key];

				param_vals.pop_relative_path();
				param_vals.pop_relative_path();
				param_vals.pop_relative_path();
			}
			else
			{
				data.controller_factories[controller_id] = std::move(controller);
				inst_data.controller_id = controller_id;
				++controller_id;
			}

			param_vals.pop_relative_path();

			data.agents.push_back(inst_data);
		}

		param_vals.pop_relative_path();

		return data;
	}

	std::vector< std::string > phys_system::get_state_values(prm::param_accessor param_vals)
	{
		std::vector< std::string > system_values;
		// System ones
		system_values.emplace_back("Time");

		auto scenario_values = phys_scenario::get_state_values(param_vals);
		system_values.insert(std::end(system_values),
			std::begin(scenario_values),
			std::end(scenario_values));
		return system_values;
	}

	std::vector< std::string > phys_system::get_agent_type_names()
	{
		return{ std::begin(agent_body_spec::Names), std::end(agent_body_spec::Names) };
	}

	std::vector< std::string > phys_system::get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals)
	{
		auto type = agent_type.as< agent_body_spec::Type >();
		return agent_body_spec::sensor_inputs(type);
	}

	size_t phys_system::get_agent_num_effectors(prm::param agent_type)
	{
		auto type = agent_type.as< agent_body_spec::Type >();
		return agent_body_spec::num_effectors(type);
	}


	phys_system::phys_system(std::unique_ptr< phys_scenario > scenario, double hertz):
		m_scenario(std::move(scenario))
	{
		m_hertz = hertz;
	}

	phys_system::~phys_system()
	{
		release_body_user_data();
	}

	void phys_system::release_body_user_data()
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

	bool phys_system::is_instantaneous() const
	{
		return false;
	}

	i_system::update_info phys_system::get_update_info() const
	{
		update_info uinf;
		uinf.type = update_info::Type::Realtime;
		uinf.frequency = m_hertz;
		return uinf;
	}


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

	void phys_system::initialize()
	{
		m_state = state();
		m_world/*m_state.world*/ = std::make_unique< b2World >(b2Vec2(0.0f, 0.0f));
		m_world/*m_state.world*/->SetContactListener(this);

		m_world_destructor = std::make_unique< phys_world_destruction_listener >();
		m_world->SetDestructionListener(m_world_destructor.get());

		auto scenario_init = m_scenario->generate_initial_state(m_rgen);
		m_scenario->load_initial_state(scenario_init, m_world.get());// m_state);

		m_td = trial_data();
		m_td.initial_st = m_state;


		/*		m_agent_update_time. = 0;
				m_b2d_update_time = 0;
				m_observer_update_time = 0;
				*/
	}

	void phys_system::clear_agents()
	{
#if 0
		m_agent = boost::none;
#endif
	}

	boost::optional< agent_id_t > phys_system::register_agent(boost::shared_ptr< i_controller > agent)
	{
		// For now, allowing only a single agent in the system
#if 0
		if(m_agent)
		{
#endif
			return boost::none;
#if 0
		}
		else
		{
			m_agent = boost::static_pointer_cast< i_phys_controller >(agent);
			return boost::optional< agent_id_t >((agent_id_t)0);
		}
#endif
	}

	std::unique_ptr< agent_body > phys_system::create_agent_from_specification(phys_agent_specification const& spec)
	{
		auto type = spec.spec_acc["spec_type"][0].as< agent_body_spec::Type >();
		std::unique_ptr< agent_body > agent;
		switch(type)
		{
			case agent_body_spec::Type::TestCreature:
			agent = std::make_unique< test_body >(spec, this);
			break;

			case agent_body_spec::Type::MinimalBiped:
			agent = std::make_unique< minimal_biped_body >(spec, this);
			break;

			case agent_body_spec::Type::TestBiped:
			agent = std::make_unique< test_biped_body >(spec, this);
			break;

			case agent_body_spec::Type::TestQuadruped:
			agent = std::make_unique< test_quadruped_body >(spec, this);
			break;

			case agent_body_spec::Type::Spaceship:
			agent = std::make_unique< basic_spaceship >(spec, this);
			break;

			default:
			throw std::exception("Invalid Agent Type");
		}

		auto init_orientation = prm::extract_fixed_or_random(spec.inst_acc["init_orientation"]).get(m_rgen);
		init_orientation *= b2_pi / 180;
		agent->rotate(init_orientation);

		auto init_x = prm::extract_fixed_or_random(spec.inst_acc["init_x"]).get(m_rgen);
		auto init_y = prm::extract_fixed_or_random(spec.inst_acc["init_y"]).get(m_rgen);
		agent->translate(b2Vec2(init_x, init_y));

		auto motion_angle = prm::extract_fixed_or_random(spec.inst_acc["init_linear_angle"]).get(m_rgen);
		motion_angle *= b2_pi / 180;
		auto linear_speed = prm::extract_fixed_or_random(spec.inst_acc["init_linear_speed"]).get(m_rgen);
		agent->set_linear_velocity(b2Vec2(
			-std::sin(motion_angle) * linear_speed,
			std::cos(motion_angle) * linear_speed
			));

		auto angular_vel = prm::extract_fixed_or_random(spec.inst_acc["init_ang_vel"]).get(m_rgen);
		angular_vel *= b2_pi / 180;
		agent->set_angular_velocity(angular_vel);

		agent->activate_effectors(std::vector< double >(agent->get_num_effectors(), 0.0));

		return agent;
	}

	i_system::agent_id phys_system::register_agent(agent_specification const& spec)//, std::unique_ptr< i_controller >&& controller)
	{
		i_system::agent_id id = m_agents.size();

		auto agent = create_agent_from_specification(boost::any_cast<phys_agent_specification const&>(spec));
		m_agents.emplace_back(agent_instance{ std::move(agent), nullptr });

		return id;
	}

	void phys_system::register_agent_controller(agent_id agent, std::unique_ptr< i_controller > controller)
	{
		m_agents[agent].controller = std::move(controller);
	}

	i_agent const& phys_system::get_agent(agent_id id) const
	{
		return *m_agents[id].agent;
	}

	i_controller const& phys_system::get_agent_controller(agent_id id) const
	{
		return *m_agents[id].controller;
	}


#if 0
	boost::any phys_system::generate_initial_state() const
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
	void phys_system::set_state(boost::any const& st)
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

	void phys_system::register_interactive_input(interactive_input const& input)
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

	bool phys_system::update(i_observer* obs)
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

		m_state.time += timestep;

		if(obs)
		{
			auto start_ = std::chrono::high_resolution_clock::now();

			//i_phys_observer* phys_obs = (i_phys_observer*)obs;
			obs->update(this);

			auto end_ = std::chrono::high_resolution_clock::now();
			m_observer_update_time += end_ - start_;
		}

		m_td.final_st = m_state;

		return !m_scenario->is_complete(m_state);
	}

	void phys_system::on_contact(b2Contact* contact, ContactType ctype)
	{
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
	}

	void phys_system::BeginContact(b2Contact* contact)
	{
		on_contact(contact, ContactType::Begin);
	}

	void phys_system::EndContact(b2Contact* contact)
	{
		on_contact(contact, ContactType::End);
	}

	void phys_system::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
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
	}


	i_observer::observations_t phys_system::record_observations(i_observer* obs) const
	{
		//i_phys_observer* phys_obs = (i_phys_observer*)obs;
		return obs->record_observations(m_td);
	}

	std::shared_ptr< i_properties const > phys_system::get_state_properties() const
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

	std::shared_ptr< i_property_values const > phys_system::get_state_property_values() const
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

	double phys_system::get_state_value(state_value_id id) const
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

	std::unique_ptr< i_system_drawer > phys_system::get_drawer() const
	{
		return std::make_unique< phys_system_drawer >(*this);
	}


	void phys_system::concatenate_performance_data(perf_data_t& pd) const
	{
		pd["Agents"] += m_agent_update_time;
		pd["Box2D"] += m_b2d_update_time;
		pd["Observers"] += m_observer_update_time;
	}

}

