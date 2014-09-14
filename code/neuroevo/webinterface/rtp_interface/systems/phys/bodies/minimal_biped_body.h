// minimal_biped_body.h

#ifndef __RTP_PHYS_MINIMAL_BIPED_BODY_H
#define __RTP_PHYS_MINIMAL_BIPED_BODY_H

#include "../rtp_phys_agent_body_spec.h"
#include "composite_rigid_body.h"


class b2RevoluteJoint;

namespace rtp {

	class phys_system;
	struct phys_agent_specification;

	class minimal_biped_body: public composite_rigid_body
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
				LeftFootContact,
				RightFootContact,
				Damage,	// TODO: individual limb damage

				_Next,
			};

			static const double DefaultMaxKneeTorque;
			static const double DefaultMaxHipTorque;

		public:
			static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
			static spec* create_instance(prm::param_accessor param);

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
			bool contact_damage = false;

			friend class minimal_biped_body;
		};

	public:
		minimal_biped_body(phys_agent_specification const& spec, phys_system* system);

	public:
		virtual agent_sensor_name_list get_sensors() const { return spec::sensor_inputs(); }
		virtual size_t get_num_effectors() const { return spec::num_effectors(); }

		virtual double get_sensor_value(agent_sensor_id const& sensor) const;
		virtual void activate_effectors(std::vector< double > const& activations);

		virtual void on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type);

	public:
		float m_pelvis_height;
		float m_pelvis_width;
		float m_upper_len;
		float m_upper_width;
		float m_lower_len;
		float m_lower_width;
		float m_foot_radius;

		double m_max_knee_torque;
		double m_max_hip_torque;

		bool m_contact_damage;

		b2Body* pelvis;
		b2Body* upper1;
		b2Body* upper2;
		b2Body* lower1;
		b2Body* lower2;
		b2RevoluteJoint* hip1;
		b2RevoluteJoint* hip2;
		b2RevoluteJoint* knee1;
		b2RevoluteJoint* knee2;

		bool left_foot_contact;
		bool right_foot_contact;

		double damage;
		double life;
	};

}


#endif


