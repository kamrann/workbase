// rtp_phys_system.cpp

#include "rtp_phys_system.h"
#include "rtp_phys_systemstatevalues.h"
#include "rtp_phys_scenario.h"
#include "rtp_phys_system_drawer.h"
#include "rtp_phys_observers.h"
#include "rtp_phys_composite_observer.h"
#include "rtp_phys_objectives.h"
#include "rtp_phys_resultant_objective.h"
#include "rtp_phys_agent_body_spec.h"
#include "rtp_phys_agent_body.h"
#include "rtp_phys_controller.h"
#include "rtp_phys_evolvable_controller.h"
#include "../rtp_interactive_agent.h"
#include "../rtp_interactive_input.h"
#include "../../params/boolean_par.h"
#include "../../params/paramlist_par.h"
#include "../../rtp_properties.h"
#include "../../rtp_pop_wide_observer.h"
#include "../../rtp_single_objective.h"
#include "../../rtp_pareto.h"

#include "wt_param_widgets/pw_yaml.h"

#include <Wt/WComboBox>


namespace rtp_phys {

	// TODO: May want to eventually separate i_agent into i_agent & i_agent_controller
	// (So we specify agent properties (such as ship configuration), and then subsequently specify a compatible controller (with its own properties)

	std::string const StateValueNames[] = {
		"Time",
		"Agent Position X",
		"Agent Position Y",
		"Agent Vel X",
		"Agent Vel Y",
		"Agent Speed",
//		"Agent Angle",
		"Agent Kinetic Energy",
	};

	void set_state_prop(rtp_stored_property_values* pv, StateValue sv, boost::any v)
	{
		pv->set_value(StateValueNames[sv], v);
	}


	enum {
		_Scenario = 0,
		_BodyType,
		_BodyParams,
		_ControllerType,
		_ControllerParams,

		_NonEvolvableParamCount,

		_ObjectiveType = _NonEvolvableParamCount,
		_ObjectiveParams,

		_EvolvableParamCount,
	};

	phys_system::param_type::param_type(bool evolvable): m_evolvable(evolvable)
	{
		add_dependency("Objective Type", _ObjectiveParams);
	}

	size_t phys_system::param_type::provide_num_child_params(rtp_param_manager* mgr) const
	{
		return m_evolvable ? _EvolvableParamCount : _NonEvolvableParamCount;
	}

	rtp_named_param phys_system::param_type::provide_child_param(size_t index, rtp_param_manager* mgr) const
	{
		switch(index)
		{
			case _Scenario:
			return rtp_named_param(phys_scenario::params());

			case _BodyType:
			return rtp_named_param(new agent_body_spec::enum_param_type(), "Spec");

			case _BodyParams:
			{
				agent_body_spec::Type t = boost::any_cast<agent_body_spec::Type>(mgr->retrieve_param("Spec"));
				return rtp_named_param(agent_body_spec::params(t));
			}

			case _ControllerType:
			return m_evolvable ? rtp_named_param(new evolvable_controller::enum_param_type(), "Controller") : rtp_named_param(new i_phys_controller::enum_param_type(), "Controller");

			case _ControllerParams:
			if(m_evolvable)
			{
				evolvable_controller::Type ea = boost::any_cast<evolvable_controller::Type>(mgr->retrieve_param("Controller"));
				return rtp_named_param(evolvable_controller::params(ea));
			}
			else
			{
				i_phys_controller::Type a = boost::any_cast<i_phys_controller::Type>(mgr->retrieve_param("Controller"));
				return rtp_named_param(i_phys_controller::params(a));
			}

			case _ObjectiveType:
			return rtp_named_param(new i_population_wide_observer::enum_param_type(), "Objective Type");

			case _ObjectiveParams:
			{
				i_population_wide_observer::Type t = boost::any_cast<i_population_wide_observer::Type>(mgr->retrieve_param("Objective Type"));
				switch(t)
				{
					case i_population_wide_observer::Single:
					return rtp_named_param(new agent_objective::enum_param_type(), "Objective");

					case i_population_wide_observer::Pareto:
					return rtp_named_param(new agent_objective::multi_param_type(), "");

					default:
					assert(false);
					return rtp_named_param();
				}
			}

			default:
			return rtp_named_param();
		}
	}

	rtp_param_type* phys_system::params(bool evolvable)
	{
		return new param_type(evolvable);
	}

	namespace sb = prm::schema;

	YAML::Node phys_system::get_schema(YAML::Node const& param_vals, bool evolvable)
	{
		auto schema = sb::list("physics2d_params");

		auto scenario = phys_scenario::get_schema(param_vals);
		sb::label(scenario, "Scenario");
		sb::append(schema, scenario);

		auto spec = agent_body_spec::get_schema(param_vals);
		sb::label(spec, "Body Spec");
		sb::append(schema, spec);

		if(evolvable)
		{
			sb::append(schema, evolvable_controller::get_schema(param_vals));
			sb::append(schema, resultant_objective::get_schema(param_vals));
		}
		else
		{
			sb::append(schema, i_phys_controller::get_schema(param_vals));
		}

		return schema;
	}

	std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > phys_system::create_instance(rtp_param param, bool evolvable)
	{
		std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > result;

		rtp_param_list param_list = boost::any_cast<rtp_param_list>(param);

		phys_scenario* scenario = phys_scenario::create_instance(param_list[_Scenario]);
		
		auto bdy_type = boost::any_cast<agent_body_spec::Type>(param_list[_BodyType]);
		agent_body_spec* spec = agent_body_spec::create_instance(bdy_type, param_list[_BodyParams]);

		std::get< 0 >(result) = new phys_system(scenario, spec);
		if(evolvable)
		{
			evolvable_controller::Type ea_type = boost::any_cast<evolvable_controller::Type>(param_list[_ControllerType]);
			// TODO: Some easy way to access sub-tuple? (eg. result.firstN)
			std::tie(
				std::get< 1 >(result),
				std::get< 2 >(result)
				) = evolvable_controller::create_instance(ea_type, param_list[_ControllerParams]);

			std::set< agent_objective::Type > required_observations;
			i_population_wide_observer::Type pwot = boost::any_cast<i_population_wide_observer::Type>(param_list[_ObjectiveType]);
			std::get< 4 >(result) = resultant_objective::create_instance(pwot, param_list[_ObjectiveParams], required_observations);

			std::get< 3 >(result) = composite_observer::create_instance(required_observations);
		}
		else
		{
			i_phys_controller::Type a_type = boost::any_cast<i_phys_controller::Type>(param_list[_ControllerType]);
			boost::shared_ptr< i_phys_controller > agent(i_phys_controller::create_instance(a_type, param_list[_ControllerParams]));
			std::get< 0 >(result)->register_agent(agent);
		}
		return result;
	}

	std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > phys_system::create_instance(YAML::Node const& param, bool evolvable)
	{
		std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > result;

		phys_scenario* scenario = phys_scenario::create_instance(param);
		agent_body_spec* spec = agent_body_spec::create_instance(param);

		std::get< 0 >(result) = new phys_system(scenario, spec);
		if(evolvable)
		{
			// TODO: Some easy way to access sub-tuple? (eg. result.firstN)
			std::tie(
				std::get< 1 >(result),
				std::get< 2 >(result)
				) = evolvable_controller::create_instance(param);

			std::set< agent_objective::Type > required_observations;
			std::get< 4 >(result) = resultant_objective::create_instance(param, required_observations);
			std::get< 3 >(result) = composite_observer::create_instance(required_observations);
		}
		else
		{
			boost::shared_ptr< i_phys_controller > agent(i_phys_controller::create_instance(param));
			std::get< 0 >(result)->register_agent(agent);
		}
		return result;
	}


	phys_system::phys_system(phys_scenario* scenario, agent_body_spec* spec)
	{
		m_scenario = scenario;
		m_body_spec = spec;
	}

	void phys_system::clear_agents()
	{
		m_agent = boost::none;
	}

	boost::optional< agent_id_t > phys_system::register_agent(boost::shared_ptr< i_agent > agent)
	{
		// For now, allowing only a single agent in the system
		if(m_agent)
		{
			return boost::none;
		}
		else
		{
			m_agent = boost::static_pointer_cast< i_phys_controller >(agent);
			return boost::optional< agent_id_t >((agent_id_t)0);
		}
	}

	boost::any phys_system::generate_initial_state(rgen_t& rgen) const
	{
		// TODO: Currently working on assumption that this does not depend on which controllers are registered, so that can be called
		// once before start of trial, and used for any subsequently registered controller

		serializable_initial_state init_st;

		init_st.scenario = m_scenario->generate_initial_state(rgen);

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
		
		return boost::any(init_st);
	}

	void phys_system::set_state(boost::any const& st)
	{
		serializable_initial_state const& init_st = boost::any_cast<serializable_initial_state const&>(st);

		m_state = state();
		m_state.world.reset(new b2World(b2Vec2(0.0f, 0.0f)));

		m_scenario->load_initial_state(init_st.scenario, m_state);

		m_state.body.reset(m_body_spec->create_body(m_state.world.get()));

		m_state.body->rotate(init_st.body_angle);
		m_state.body->translate(init_st.body_pos);
		m_state.body->set_linear_velocity(init_st.body_lin_vel);
		m_state.body->set_angular_velocity(init_st.body_ang_vel);

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

	void phys_system::register_interactive_input(interactive_input const& input)
	{
		// TODO: Maybe store within create_instance a pointer to any interactive controller, or provide a method on i_agent
		// along lines of is_interactive()
		if(m_agent)
		{
			boost::dynamic_pointer_cast<i_interactive_agent>(*m_agent)->register_input(input);
		}
	}

	bool phys_system::update(i_observer* obs)
	{
		float const hertz = 60.0f;
		float const timestep = 1.0f / hertz;

		/*
		This should generalise to something like:

		sys_update_data sud
		for each agent a:							// a is agent of unknown type
			register_agent_actions(a(state), sud)	// This uses virtual mechanism to determine what effect agent a's actions will have on system

		process_system_updates(sud)					// Update system taking into account all agents' actions simultaneously

		This way, we can have different types of agents, which can affect the system in different ways.
		We no longer have a single decision type, but rather let agent type specific code register whatever effects it needs on the system.
		*/

		(*m_agent)->update(m_state, m_scenario->get_scenario_data());
		
		m_state.world->Step(timestep, 8, 3);	// TODO: iterations

		m_state.time += timestep;

		if(obs)
		{
			i_phys_observer* phys_obs = (i_phys_observer*)obs;
			phys_obs->update(m_state);
		}

		m_td.final_st = m_state;

		return !m_scenario->is_complete(m_state);
	}

	i_observer::observations_t phys_system::record_observations(i_observer* obs) const
	{
		i_phys_observer* phys_obs = (i_phys_observer*)obs;
		return phys_obs->record_observations(m_td);
	}

	boost::shared_ptr< i_properties const > phys_system::get_state_properties() const
	{
		rtp_stored_properties* props = new rtp_stored_properties();
		for(size_t i = 0; i < StateValue::Count; ++i)
		{
			props->add_property(StateValueNames[i]);
		}
		return boost::shared_ptr< i_properties const >(props);
	}

	boost::shared_ptr< i_property_values const > phys_system::get_state_property_values() const
	{
		rtp_stored_property_values* vals = new rtp_stored_property_values();
		set_state_prop(vals, Time, m_state.time);
		b2Vec2 pos = m_state.body->get_position();
		set_state_prop(vals, AgentPosX, pos.x);
		set_state_prop(vals, AgentPosY, pos.y);
		b2Vec2 vel = m_state.body->get_linear_velocity();
		set_state_prop(vals, AgentVelX, vel.x);
		set_state_prop(vals, AgentVelY, vel.y);
		set_state_prop(vals, AgentSpeed, vel.Length());
//		set_state_prop(vals, AgentAngle, m_state.body->get_orientation());
		set_state_prop(vals, AgentKE, m_state.body->get_kinetic_energy());
		return boost::shared_ptr< i_property_values const >(vals);
	}

	i_system_drawer* phys_system::get_drawer() const
	{
		return new phys_system_drawer(*this);
	}
}

