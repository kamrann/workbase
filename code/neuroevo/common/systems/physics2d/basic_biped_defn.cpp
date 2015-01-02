// basic_biped_defn.cpp

#include "basic_biped_defn.h"
#include "basic_biped.h"


namespace sys {
	namespace phys2d {

		bimap< basic_biped_defn::Joint, std::string > const basic_biped_defn::s_joint_names = {
				{ Joint::LeftKnee, "left_knee" },
				{ Joint::RightKnee, "right_knee" },
				{ Joint::LeftHip, "left_hip" },
				{ Joint::RightHip, "right_hip" },
				{ Joint::Pelvic, "pelvic" },
				{ Joint::LeftElbow, "left_elbow" },
				{ Joint::RightElbow, "right_elbow" },
				{ Joint::LeftShoulder, "left_shoulder" },
				{ Joint::RightShoulder, "right_shoulder" },
		};

		bimap< basic_biped_defn::JointStateValue, std::string > const basic_biped_defn::s_joint_sv_names = {
				{ JointStateValue::Angle, "angle" },
				{ JointStateValue::Speed, "speed" },
				{ JointStateValue::Energy, "energy" },
				{ JointStateValue::Torque, "torque" },
		};


		std::string basic_biped_defn::get_name() const
		{
			return "basic_biped";
		}

		ddl::defn_node basic_biped_defn::get_spec_defn(ddl::specifier& spc)
		{
/* TODO:			(*provider)[path + std::string("upper_body")] = [=](prm::param_accessor)
			{
				auto s = sb::boolean("upper_body", false);
				sb::label(s, "Upper Body");
				return s;
			};
*/
			float const MaxPelvicTorque = 125.0f;
			float const MaxHipTorque = 75.0f;
			float const MaxKneeTorque = 75.0f;
			float const MaxShoulderTorque = 40.0f;
			float const MaxElbowTorque = 40.0f;
			float const MaxNeckTorque = 20.0f;

			ddl::defn_node max_torques = spc.composite("max_torques")(ddl::define_children{}
				("knee", spc.realnum("knee")(ddl::spc_default < ddl::realnum_defn_node::value_t > { MaxKneeTorque }))
				("hip", spc.realnum("hip")(ddl::spc_default < ddl::realnum_defn_node::value_t > { MaxHipTorque }))
/*				("elbow", spc.realnum()(ddl::spc_default < ddl::realnum_defn_node::value_t > { MaxElbowTorque }))
				("shoulder", spc.realnum()(ddl::spc_default < ddl::realnum_defn_node::value_t > { MaxShoulderTorque }))
				("pelvic", spc.realnum()(ddl::spc_default < ddl::realnum_defn_node::value_t > { MaxPelvicTorque }))
				("neck", spc.realnum()(ddl::spc_default < ddl::realnum_defn_node::value_t > { MaxNeckTorque }))
*/				);

			ddl::defn_node contact_damage = spc.boolean("contact_damage")
				(ddl::spc_default < ddl::boolean_defn_node::value_t > { false })
				;

			agent_state_values_fn_.set_fn([this](ddl::navigator nav)
			{
				auto svs = std::vector < state_value_id > {};
				state_value_id sv_id;

				// First, the state values inherited from composite_body
				auto object_state_vals = composite_body::get_state_values();
				svs.insert(std::end(svs), std::begin(object_state_vals), std::end(object_state_vals));

				// Then biped specific state values
				composite_body_defn comp_defn;

				auto has_upper = false;// TODO: prm::extract_as< bool >(acc["upper_body"]);
				auto joints = has_upper ? all_joints() : lower_joints();
				for(auto jnt : joints)
				{
					comp_defn.add_revolute(s_joint_names.at(jnt));
					/*
					sv_id += s_joint_names.at(jnt);

					for(auto jnt_sv_i = 0; jnt_sv_i < (int)JointStateValue::Count; ++jnt_sv_i)
					{
					auto jnt_sv = (JointStateValue)jnt_sv_i;

					sv_id += s_joint_sv_names.at(jnt_sv);

					svs.push_back(sv_id);

					sv_id.pop();
					}

					sv_id.pop();
					*/
				}

				auto composite_state_vals = comp_defn.get_state_values();
				svs.insert(std::end(svs), std::begin(composite_state_vals), std::end(composite_state_vals));

				svs.push_back(state_value_id::from_string("left_foot_contact"));
				svs.push_back(state_value_id::from_string("right_foot_contact"));
				svs.push_back(state_value_id::from_string("both_foot_contact"));

				return svs;
			});

			num_effectors_fn_.set_fn([this](ddl::navigator nav)
			{
				// TODO: let composite_body_defn determine the number of joint effectors

				size_t count = 4;	// 2 knees, 2 hips
				auto has_upper = false;// TODO: nav["upper_body"]);
				if(has_upper)
				{
					count += 0;	// TODO:
				}
				return count;
			});
			// TODO: upper body num_effectors_fn_.add_dependency();

			return spc.composite("basic_biped")(ddl::define_children{}
				// TODO: upper body
				("max_torques", max_torques)
				("contact_damage", contact_damage)
				);
		}

		ddl::dep_function< state_value_id_list > basic_biped_defn::get_agent_state_values_fn() const
		{
			return agent_state_values_fn_;
		}

		state_value_id_list basic_biped_defn::get_agent_state_values(ddl::navigator nav) const
		{
			return agent_state_values_fn_(nav);
		}

		std::vector< std::string > basic_biped_defn::sensor_inputs(ddl::navigator nav) const
		{
			return{};
		}

		ddl::dep_function< size_t > basic_biped_defn::num_effectors_fn() const
		{
			return num_effectors_fn_;
		}

		size_t basic_biped_defn::num_effectors(ddl::navigator nav) const
		{
			return num_effectors_fn_(nav);
		}

		agent_ptr basic_biped_defn::create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const
		{
			auto spec = std::make_shared< spec_data >();
			// TODO: params
			spec->pelvis_height = 1.0f;
			spec->pelvis_width = 1.0f;
			spec->upper_len = 2.0f;
			spec->upper_width = 0.5f;
			spec->lower_len = 2.0f;
			spec->lower_width = 0.4f;
			spec->foot_radius = 0.4f;

			auto max_torques = spec_nav["max_torques"];
			auto max_knee = max_torques["knee"].get().as_real();
			spec->joint_data[basic_biped_defn::Joint::LeftKnee].max_torque = max_knee;
			spec->joint_data[basic_biped_defn::Joint::RightKnee].max_torque = max_knee;
				
			auto max_hip = max_torques["hip"].get().as_real();
			spec->joint_data[basic_biped_defn::Joint::LeftHip].max_torque = max_hip;
			spec->joint_data[basic_biped_defn::Joint::RightHip].max_torque = max_hip;

			/* TODO:
			auto has_upper = prm::extract_as< bool >(spec_acc["upper_body"]);
			if(has_upper)
			{
				// todo:
			}
			*/

			spec->contact_damage = spec_nav["contact_damage"].get().as_bool();

			auto inst = std::make_shared< instance_data >();
			phys2d_agent_defn::initialize_instance_data(*inst, inst_nav);

			return std::make_unique< basic_biped >(spec, inst);
		}

	}
}


