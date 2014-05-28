// rtp_phys_agent_body_spec.cpp

#include "rtp_phys_agent_body_spec.h"
#include "../../rtp_defs.h"
#include "bodies/test_body.h"
#include "bodies/test_quadruped_body.h"
#include "bodies/test_biped_body.h"
#include "bodies/basic_spaceship.h"
#include "../../params/fixed_or_random_par.h"


namespace rtp_phys {

	std::string const agent_body_spec::Names[] = {
		"Test Creature",
		"Test Quadruped",
		"Test Biped",
		"Spaceship",
	};

	agent_body_spec::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	rtp_named_param_list agent_body_spec::base_params()
	{
		rtp_named_param_list p;
		p.push_back(rtp_named_param(
			new rtp_fixed_or_random_param_type(
			0.0,
			-10.0,
			10.0),
			"Initial Position X"));
		p.push_back(rtp_named_param(
			new rtp_fixed_or_random_param_type(
			0.0,
			-10.0,
			10.0),
			"Initial Position Y"));
		p.push_back(rtp_named_param(
			new rtp_fixed_or_random_param_type(
			0.0,
			-180,
			180,
			-180,
			180),
/*			-boost::math::double_constants::pi,
			boost::math::double_constants::pi,
			-boost::math::double_constants::pi,
			boost::math::double_constants::pi),
*/			"Initial Orientation"));
		p.push_back(rtp_named_param(
			new rtp_fixed_or_random_param_type(
			0.0,
			0.0,
			//2 * boost::math::double_constants::pi,
			360,
			0.0,
			//2 * boost::math::double_constants::pi
			360
			),
			"Initial Linear Motion Angle"));
		p.push_back(rtp_named_param(
			new rtp_fixed_or_random_param_type(
			0.0,
			0.0,
			10.0,
			0.0),
			"Initial Linear Speed"));
		p.push_back(rtp_named_param(
			new rtp_fixed_or_random_param_type(
			0.0,
			-90.0,
			90.0),
			"Initial Angular Vel"));
		return p;
	}

	void agent_body_spec::create_base_instance(rtp_param param, agent_body_spec* s)
	{
		auto param_list = boost::any_cast<rtp_param_list>(param);
		typedef fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > f_or_r_t;
		s->m_initial_pos_x = boost::any_cast<f_or_r_t>(param_list[0]);
		s->m_initial_pos_y = boost::any_cast<f_or_r_t>(param_list[1]);
		s->m_initial_orientation = boost::any_cast<f_or_r_t>(param_list[2]);
		s->m_initial_linear_dir = boost::any_cast<f_or_r_t>(param_list[3]);
		s->m_initial_linear_speed = boost::any_cast<f_or_r_t>(param_list[4]);
		s->m_initial_ang_vel = boost::any_cast<f_or_r_t>(param_list[5]);
	}

	rtp_param_type* agent_body_spec::params(Type type)
	{
		switch(type)
		{
			case TestCreature:
			return test_body::spec::params();

			case TestQuadruped:
			return test_quadruped_body::spec::params();

			case TestBiped:
			return test_biped_body::spec::params();

			case Spaceship:
			return basic_spaceship::spec::params();

			default:
			assert(false);
			return nullptr;
		}
	}

	agent_body_spec* agent_body_spec::create_instance(Type type, rtp_param param)
	{
		switch(type)
		{
			case TestCreature:
			return test_body::spec::create_instance(param);

			case TestQuadruped:
			return test_quadruped_body::spec::create_instance(param);

			case TestBiped:
			return test_biped_body::spec::create_instance(param);

			case Spaceship:
			return basic_spaceship::spec::create_instance(param);

			default:
			return nullptr;
		}
	}

	agent_body_spec::agent_body_spec(Type t): m_type(t)
	{}
}



