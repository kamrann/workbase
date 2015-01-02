// pole_cart_defn.cpp

#include "pole_cart_defn.h"
#include "pole_cart.h"


namespace sys {
	namespace phys2d {

		bimap< pole_cart_defn::Joint, std::string > const pole_cart_defn::s_joint_names = {
				{ Joint::Pole, "pole_joint" },
		};

		bimap< pole_cart_defn::JointStateValue, std::string > const pole_cart_defn::s_joint_sv_names = {
				{ JointStateValue::Angle, "angle" },
				{ JointStateValue::Speed, "speed" },
		};


		std::string pole_cart_defn::get_name() const
		{
			return "pole_cart";
		}

		ddl::defn_node pole_cart_defn::get_spec_defn(ddl::specifier& spc)
		{
			float const DefaultCartMass = 1.0f;
			float const DefaultPoleMass = 0.1f;
			float const DefaultPoleLength = 1.0f;
			float const DefaultMaxForce = 10.0f;
			float const DefaultFailAngle = b2_pi / 15.0f;
			float const DefaultFailDisplacement = 2.4f;

			ddl::defn_node cart_mass = spc.realnum("cart_mass")
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { DefaultCartMass })
				;
			ddl::defn_node pole_mass = spc.realnum("pole_mass")
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { DefaultPoleMass })
				;
			ddl::defn_node pole_length = spc.realnum("pole_length")
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { DefaultPoleLength })
				;
			ddl::defn_node max_force = spc.realnum("max_force")
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { DefaultMaxForce })
				;
			ddl::defn_node fail_angle = spc.realnum("fail_angle")
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { DefaultFailAngle })
				;
			ddl::defn_node fail_displacement = spc.realnum("fail_displacement")
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { DefaultFailDisplacement })
				;

			agent_state_values_fn_.set_fn([this](ddl::navigator nav)
			{
				auto svs = std::vector < state_value_id > {};
				state_value_id sv_id;

				// First, the state values inherited from composite_body
				auto object_state_vals = composite_body::get_state_values();
				svs.insert(std::end(svs), std::begin(object_state_vals), std::end(object_state_vals));

				// Then pole cart specific state values
				auto joints = all_joints();
				for(auto jnt : joints)
				{
					sv_id += s_joint_names.at(jnt);

					for(auto jnt_sv : std::set < JointStateValue >{ JointStateValue::Angle, JointStateValue::Speed })
					{
						sv_id += s_joint_sv_names.at(jnt_sv);

						svs.push_back(sv_id);

						sv_id.pop();
					}

					sv_id.pop();
				}

				svs.push_back(state_value_id::from_string("abs_pole_angle"));
				svs.push_back(state_value_id::from_string("abs_displacement"));
				svs.push_back(state_value_id::from_string("failure"));

				return svs;
			});

			num_effectors_fn_.set_fn([this](ddl::navigator nav)
			{
				return 1;
			});

			return spc.composite("pole_cart")(ddl::define_children{}
				("cart_mass", cart_mass)
				("pole_mass", pole_mass)
				("pole_length", pole_length)
				("max_force", max_force)
				("fail_angle", fail_angle)
				("fail_displacement", fail_displacement)
				);
		}

		ddl::dep_function< state_value_id_list > pole_cart_defn::get_agent_state_values_fn() const
		{
			return agent_state_values_fn_;
		}

		state_value_id_list pole_cart_defn::get_agent_state_values(ddl::navigator nav) const
		{
			return agent_state_values_fn_(nav);
		}

		std::vector< std::string > pole_cart_defn::sensor_inputs(ddl::navigator nav) const
		{
			return{};
		}

		ddl::dep_function< size_t > pole_cart_defn::num_effectors_fn() const
		{
			return num_effectors_fn_;
		}

		size_t pole_cart_defn::num_effectors(ddl::navigator nav) const
		{
			return num_effectors_fn_(nav);
		}

		agent_ptr pole_cart_defn::create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const
		{
			auto spec = std::make_shared< spec_data >();

			spec->cart_mass = spec_nav["cart_mass"].get().as_real();
			spec->pole_mass = spec_nav["pole_mass"].get().as_real();
			spec->pole_length = spec_nav["pole_length"].get().as_real();
			spec->max_force = spec_nav["max_force"].get().as_real();
			spec->fail_angle = spec_nav["fail_angle"].get().as_real();
			spec->fail_displacement = spec_nav["fail_displacement"].get().as_real();

			auto inst = std::make_shared< instance_data >();
			phys2d_agent_defn::initialize_instance_data(*inst, inst_nav);

			return std::make_unique< pole_cart >(spec, inst);
		}

	}
}


