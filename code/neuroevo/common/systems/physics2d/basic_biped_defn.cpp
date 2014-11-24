// basic_biped_defn.cpp

#include "basic_biped_defn.h"
#include "basic_biped.h"

#include "params/schema_builder.h"
#include "params/param_accessor.h"


namespace sb = prm::schema;

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
		};


		std::string basic_biped_defn::get_name() const
		{
			return "basic_biped";
		}

		std::string basic_biped_defn::update_schema_providor_for_spec(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

			(*provider)[path + std::string("upper_body")] = [=](prm::param_accessor)
			{
				auto s = sb::boolean("upper_body", false);
				sb::label(s, "Upper Body");
				return s;
			};

			path += std::string{ "max_torques" };

			float const MaxPelvicTorque = 125.0f;
			float const MaxHipTorque = 75.0f;
			float const MaxKneeTorque = 75.0f;
			float const MaxShoulderTorque = 40.0f;
			float const MaxElbowTorque = 40.0f;
			float const MaxNeckTorque = 20.0f;

			(*provider)[path + std::string("max_knee_torque")] = [=](prm::param_accessor)
			{
				auto s = sb::real("max_knee_torque", MaxKneeTorque);
				sb::label(s, "Knee");
				return s;
			};

			(*provider)[path + std::string("max_hip_torque")] = [=](prm::param_accessor)
			{
				auto s = sb::real("max_hip_torque", MaxHipTorque);
				sb::label(s, "Hip");
				return s;
			};

			(*provider)[path + std::string("max_elbow_torque")] = [=](prm::param_accessor)
			{
				auto s = sb::real("max_elbow_torque", MaxElbowTorque);
				sb::label(s, "Elbow");
				return s;
			};

			(*provider)[path + std::string("max_shoulder_torque")] = [=](prm::param_accessor)
			{
				auto s = sb::real("max_shoulder_torque", MaxShoulderTorque);
				sb::label(s, "Shoulder");
				return s;
			};

			(*provider)[path + std::string("max_pelvic_torque")] = [=](prm::param_accessor)
			{
				auto s = sb::real("max_pelvic_torque", MaxPelvicTorque);
				sb::label(s, "Pelvic");
				return s;
			};

			(*provider)[path + std::string("max_neck_torque")] = [=](prm::param_accessor)
			{
				auto s = sb::real("max_neck_torque", MaxNeckTorque);
				sb::label(s, "Neck");
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list("max_torques");
				sb::append(s, provider->at(path + std::string("max_knee_torque"))(acc));
				sb::append(s, provider->at(path + std::string("max_hip_torque"))(acc));
				if(acc.is_available("upper_body") && prm::extract_as< bool >(acc["upper_body"]))
				{
					sb::append(s, provider->at(path + std::string("max_elbow_torque"))(acc));
					sb::append(s, provider->at(path + std::string("max_shoulder_torque"))(acc));
					sb::append(s, provider->at(path + std::string("max_pelvic_torque"))(acc));
					sb::append(s, provider->at(path + std::string("max_neck_torque"))(acc));
				}
				sb::label(s, "Max Torques");
				return s;
			};

			path.pop();

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string("upper_body"))(acc));
				sb::append(s, provider->at(path + std::string("max_torques"))(acc));
				return s;
			};

			return path.leaf().name();
		}

		state_value_id_list basic_biped_defn::get_agent_state_values(prm::param_accessor acc) const
		{
			auto svs = std::vector < state_value_id > {};
			state_value_id sv_id;

			// First, the state values inherited from composite_body
			auto object_state_vals = composite_body::get_state_values();
			svs.insert(std::end(svs), std::begin(object_state_vals), std::end(object_state_vals));

			// Then biped specific state values
			auto has_upper = prm::extract_as< bool >(acc["upper_body"]);
			auto joints = has_upper ? all_joints() : lower_joints();
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

			svs.push_back(state_value_id::from_string("left_foot_contact"));
			svs.push_back(state_value_id::from_string("right_foot_contact"));
			svs.push_back(state_value_id::from_string("both_foot_contact"));

			return svs;
		}

		std::vector< std::string > basic_biped_defn::sensor_inputs(prm::param_accessor acc) const
		{
			return{};
		}

		size_t basic_biped_defn::num_effectors(prm::param_accessor acc) const
		{
			size_t count = 4;	// 2 knees, 2 hips
			auto has_upper = prm::extract_as< bool >(acc["upper_body"]);
			if(has_upper)
			{
				count += 0;	// TODO:
			}
			return count;
		}

		agent_ptr basic_biped_defn::create_agent(prm::param_accessor spec_acc, prm::param_accessor inst_acc) const
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

			auto max_knee = prm::extract_as< double >(spec_acc["max_knee_torque"]);
			spec->joint_data[basic_biped_defn::Joint::LeftKnee].max_torque = max_knee;
			spec->joint_data[basic_biped_defn::Joint::RightKnee].max_torque = max_knee;
				
			auto max_hip = prm::extract_as< double >(spec_acc["max_hip_torque"]);
			spec->joint_data[basic_biped_defn::Joint::LeftHip].max_torque = max_hip;
			spec->joint_data[basic_biped_defn::Joint::RightHip].max_torque = max_hip;

			auto has_upper = prm::extract_as< bool >(spec_acc["upper_body"]);
			if(has_upper)
			{
				// todo:
			}

			spec->contact_damage = false;	// TODO:

			auto inst = std::make_shared< instance_data >();
			phys2d_agent_defn::initialize_instance_data(*inst, inst_acc);

			return std::make_unique< basic_biped >(spec, inst);
		}

	}
}


