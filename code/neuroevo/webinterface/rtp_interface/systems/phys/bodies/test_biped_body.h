// test_biped_body.h

#ifndef __RTP_PHYS_TEST_BIPED_BODY_H
#define __RTP_PHYS_TEST_BIPED_BODY_H

#include "../rtp_phys_agent_body_spec.h"
#include "composite_rigid_body.h"


class b2RevoluteJoint;

namespace rtp {

	class phys_system;
	struct phys_agent_specification;

	class test_biped_body: public composite_rigid_body
	{
	public:
		class spec: public agent_body_spec
		{
		public:
			enum Sensors {
				_Prev = composite_rigid_body::Sensors::_Next - 1,

				LeftKneeAngle,
				LeftKneeSpeed,
				RightKneeAngle,
				RightKneeSpeed,
				LeftHipAngle,
				LeftHipSpeed,
				RightHipAngle,
				RightHipSpeed,
				AbdomenAngle,
				AbdomenSpeed,
				LeftElbowAngle,
				LeftElbowSpeed,
				RightElbowAngle,
				RightElbowSpeed,
				LeftShoulderAngle,
				LeftShoulderSpeed,
				RightShoulderAngle,
				RightShoulderSpeed,
				NeckAngle,
				NeckSpeed,
				Balance,		// Headorientation, 0 == level
				NonFootContactCount,	// TODO: Makes no sense to be a sensor really, more an agent state value
				HeadHeight,

				_Next,
			};

			static const double DefaultMaxKneeTorque;
			static const double DefaultMaxHipTorque;
			static const double DefaultMaxElbowTorque;
			static const double DefaultMaxShoulderTorque;
			static const double DefaultMaxPelvicTorque;
			static const double DefaultMaxNeckTorque;

		public:
			static prm::schema::schema_node get_schema(prm::param_accessor param_vals);
			static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
			static spec* create_instance(prm::param_accessor param);
			static spec* create_instance(prm::param& param);

			static agent_sensor_name_list sensor_inputs();
			static size_t num_effectors();

		public:
			virtual agent_body* create_body(b2World* world);
			virtual agent_sensor_name_list get_sensors() const { return sensor_inputs(); }
			virtual size_t get_num_effectors() const { return num_effectors(); }

		protected:
			spec();

		protected:
			double max_knee_torque = DefaultMaxKneeTorque;
			double max_hip_torque = DefaultMaxHipTorque;
			double max_elbow_torque = DefaultMaxElbowTorque;
			double max_shoulder_torque = DefaultMaxShoulderTorque;
			double max_pelvic_torque = DefaultMaxPelvicTorque;
			double max_neck_torque = DefaultMaxNeckTorque;

			friend class test_biped_body;
		};

	public:
		test_biped_body(phys_agent_specification const& spec, phys_system* system);

	public:
		virtual agent_sensor_name_list get_sensors() const { return spec::sensor_inputs(); }
		virtual size_t get_num_effectors() const { return spec::num_effectors(); }

		virtual double get_sensor_value(agent_sensor_id const& sensor) const;
		virtual void activate_effectors(std::vector< double > const& activations);

		virtual void on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type);
			//b2Contact* contact);

	public:
		float m_torso_len;
		float m_torso_width;
		float m_pelvis_height;
		float m_pelvis_width;
		float m_upper_len;
		float m_upper_width;
		float m_lower_len;
		float m_lower_width;
		float m_foot_radius;
		float m_upper_arm_len;
		float m_upper_arm_width;
		float m_forearm_len;
		float m_forearm_width;
		float m_head_radius;

		double m_max_knee_torque;
		double m_max_hip_torque;
		double m_max_elbow_torque;
		double m_max_shoulder_torque;
		double m_max_pelvic_torque;
		double m_max_neck_torque;

		b2Body* torso;
		b2Body* pelvis;
		b2Body* upper1;
		b2Body* upper2;
		b2Body* lower1;
		b2Body* lower2;
		b2Body* upper_arm1;
		b2Body* upper_arm2;
		b2Body* forearm1;
		b2Body* forearm2;
		b2Body* head;
		b2RevoluteJoint* pelvis_joint;
		b2RevoluteJoint* hip1;
		b2RevoluteJoint* hip2;
		b2RevoluteJoint* knee1;
		b2RevoluteJoint* knee2;
		b2RevoluteJoint* shoulder1;
		b2RevoluteJoint* shoulder2;
		b2RevoluteJoint* elbow1;
		b2RevoluteJoint* elbow2;
		b2RevoluteJoint* neck_joint;

//		size_t m_foot_contact_count;
		size_t m_non_foot_contact_count;
	};

}


#endif


