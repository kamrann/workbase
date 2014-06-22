// rtp_phys_agent_body_spec.cpp

#include "rtp_phys_agent_body_spec.h"
#include "../../rtp_defs.h"
#include "bodies/test_body.h"
#include "bodies/test_quadruped_body.h"
#include "bodies/test_biped_body.h"
#include "bodies/basic_spaceship.h"
#include "../../params/fixed_or_random_par.h"

#include "wt_param_widgets/pw_yaml.h"


namespace rtp_phys {

	std::string const agent_body_spec::Names[] = {
		"Test Creature",
		"Test Quadruped",
		"Test Biped",
		"Spaceship",
	};

}

namespace YAML {
	template <>
	struct convert< rtp_phys::agent_body_spec::Type >
	{
		static Node encode(rtp_phys::agent_body_spec::Type const& rhs)
		{
			return Node(rtp_phys::agent_body_spec::Names[rhs]);
		}

		static bool decode(Node const& node, rtp_phys::agent_body_spec::Type& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp_phys::agent_body_spec::Type > const mapping_;
	};

	std::map< std::string, rtp_phys::agent_body_spec::Type > const convert< rtp_phys::agent_body_spec::Type >::mapping_ = {
		{ "Test Creature", rtp_phys::agent_body_spec::Type::TestCreature },
		{ "Test Quadruped", rtp_phys::agent_body_spec::Type::TestQuadruped },
		{ "Test Biped", rtp_phys::agent_body_spec::Type::TestBiped },
		{ "Spaceship", rtp_phys::agent_body_spec::Type::Spaceship },
	};
}

namespace rtp_phys {

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

	// TODO: Temp solution
	fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > extract_fixed_or_random(prm::random const& val)
	{
		if(val.is_fixed)
		{
			return { boost::get< double >(val.range) };
		}
		else
		{
			auto r = boost::get< std::pair< double, double > >(val.range);
			return { r.first, r.second };
		}
	}

	void agent_body_spec::create_base_instance(YAML::Node const& param, agent_body_spec* s)
	{
		s->m_initial_pos_x = extract_fixed_or_random(prm::find_value(param, "Initial X Pos").as< prm::random >());
		s->m_initial_pos_y = extract_fixed_or_random(prm::find_value(param, "Initial Y Pos").as< prm::random >());
		s->m_initial_orientation = extract_fixed_or_random(prm::find_value(param, "Initial Orientation").as< prm::random >());
		s->m_initial_linear_dir = extract_fixed_or_random(prm::find_value(param, "Initial Linear Motion Angle").as< prm::random >());
		s->m_initial_linear_speed = extract_fixed_or_random(prm::find_value(param, "Initial Linear Speed").as< prm::random >());
		s->m_initial_ang_vel = extract_fixed_or_random(prm::find_value(param, "Initial Angular Velocity").as< prm::random >());
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

	namespace sb = prm::schema;

	YAML::Node agent_body_spec::get_schema(YAML::Node const& param_vals)
	{
		auto schema = sb::list("Spec");

		auto type = sb::enum_selection(
			"Spec Type",
			{ begin(Names), end(Names) }
		);
		sb::on_update(type);
		sb::append(schema, type);

		auto node = prm::find_value(param_vals, "Spec Type");
		auto enum_sel = node.IsNull() ? Type::Default : node.as< Type >();
		switch(enum_sel)
		{
			case Type::TestCreature:
			{
				sb::append(schema, test_body::spec::get_schema(param_vals/*["Test Creature Params"]*/));
			}
			break;

			case Type::TestQuadruped:
			{
				sb::append(schema, test_quadruped_body::spec::get_schema(param_vals/*["Test Quadruped Params"]*/));
			}
			break;

			case Type::TestBiped:
			{
				sb::append(schema, test_biped_body::spec::get_schema(param_vals/*["Test Biped Params"]*/));
			}
			break;

			case Type::Spaceship:
			{
				sb::append(schema, basic_spaceship::spec::get_schema(param_vals/*["Spaceship Params"]*/));
			}
			break;

			default:
			assert(false);
		}

		auto init_x = sb::random("Initial X Pos", 0.0, boost::none, boost::none, -10.0, 10.0);
		sb::append(schema, init_x);
		auto init_y = sb::random("Initial Y Pos", 0.0, boost::none, boost::none, -10.0, 10.0);
		sb::append(schema, init_y);
		auto init_orientation = sb::random("Initial Orientation", 0.0, -180.0, 180.0);
		sb::append(schema, init_orientation);
		auto init_linear_angle = sb::random("Initial Linear Motion Angle", 0.0, 0.0, 360.0);
		sb::append(schema, init_linear_angle);
		auto init_linear_speed = sb::random("Initial Linear Speed", 0.0, 0.0, boost::none, 0.0, 10.0);
		sb::append(schema, init_linear_speed);
		auto init_ang_vel = sb::random("Initial Angular Velocity", 0.0, boost::none, boost::none, -90.0, 90.0);
		sb::append(schema, init_ang_vel);

		return schema;
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

	agent_body_spec* agent_body_spec::create_instance(YAML::Node const& param)
	{
		auto type = prm::find_value(param, "Spec Type").as< Type >();
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

	std::vector< std::string > agent_body_spec::sensor_inputs(Type type)
	{
		switch(type)
		{
			case TestCreature:
			return test_body::spec::sensor_inputs();
			case TestQuadruped:
			return test_quadruped_body::spec::sensor_inputs();
			case TestBiped:
			return test_biped_body::spec::sensor_inputs();
			case Spaceship:
			return basic_spaceship::spec::sensor_inputs();

			default:
			assert(false);
			return{};
		}
	}

	agent_body_spec::agent_body_spec(Type t): m_type(t)
	{}
}



