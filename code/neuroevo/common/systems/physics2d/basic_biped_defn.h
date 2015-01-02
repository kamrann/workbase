// basic_biped_defn.h

#ifndef __NE_PHYS2D_BASIC_BIPED_DEFN_H
#define __NE_PHYS2D_BASIC_BIPED_DEFN_H

#include "phys2d_agent_defn.h"
#include "composite_body_defn.h"

#include "util/bimap.h"

#include <set>


namespace sys {
	namespace phys2d {

		class basic_biped_defn:
			public phys2d_agent_defn
			//, public composite_body_defn
		{
		public:
			virtual std::string get_name() const override;
			virtual ddl::defn_node get_spec_defn(ddl::specifier& spc) override;

			virtual ddl::dep_function< state_value_id_list >
				get_agent_state_values_fn() const override;
			virtual state_value_id_list get_agent_state_values(ddl::navigator nav) const override;

			virtual std::vector< std::string > sensor_inputs(ddl::navigator nav) const override;

			virtual ddl::dep_function< size_t > num_effectors_fn() const override;
			virtual size_t num_effectors(ddl::navigator nav) const override;

			virtual agent_ptr create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const override;

		private:
			enum class Joint {
				LeftKnee,
				RightKnee,
				LeftHip,
				RightHip,
				Pelvic,
				LeftElbow,
				RightElbow,
				LeftShoulder,
				RightShoulder,

				Count,
			};

			enum class JointStateValue {
				Angle,
				Speed,
				Energy,
				Torque,

				Count,
			};

			struct spec_data
			{
				float pelvis_height;
				float pelvis_width;
				float upper_len;
				float upper_width;
				float lower_len;
				float lower_width;
				float foot_radius;

				struct joint_spec
				{
					double max_torque;
				};

				std::map< Joint, joint_spec > joint_data;

				bool contact_damage;
			};

			typedef phys2d_agent_defn::instance_data instance_data;	// TODO: placeholder

			static auto lower_joints() -> std::set < Joint >
			{
				return{
					Joint::LeftKnee, 
					Joint::RightKnee, 
					Joint::LeftHip, 
					Joint::RightHip 
				};
			}

			static auto upper_joints() -> std::set < Joint >
			{
				return{
					Joint::Pelvic,
					Joint::LeftElbow,
					Joint::RightElbow, 
					Joint::LeftShoulder, 
					Joint::RightShoulder 
				};
			}

			static auto all_joints() -> std::set < Joint >
			{
				std::set < Joint >
					lower = lower_joints(),
					upper = upper_joints(),
					all;
				std::set_union(
					std::begin(lower), std::end(lower),
					std::begin(upper), std::end(upper),
					std::inserter(all, std::begin(all))
					);
				return all;
			}

			static bimap< Joint, std::string > const s_joint_names;
			static bimap< JointStateValue, std::string > const s_joint_sv_names;

			ddl::dep_function< state_value_id_list > agent_state_values_fn_;
			ddl::dep_function< size_t > num_effectors_fn_;

			friend class basic_biped;
		};

	}
}


#endif

