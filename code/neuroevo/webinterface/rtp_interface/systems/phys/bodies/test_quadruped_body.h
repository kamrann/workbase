// test_quadruped_body.h

#ifndef __RTP_PHYS_TEST_QUADRUPED_BODY_H
#define __RTP_PHYS_TEST_QUADRUPED_BODY_H

#include "../rtp_phys_agent_body_spec.h"
#include "composite_rigid_body.h"


class b2RevoluteJoint;

namespace rtp {

	class phys_system;
	struct phys_agent_specification;

	class test_quadruped_body: public composite_rigid_body
	{
	public:
		class spec: public agent_body_spec
		{
		public:
			enum Sensors {
				_Prev = composite_rigid_body::Sensors::_Next - 1,

				RearLeftKneeAngle,
				RearLeftKneeSpeed,
				RearRightKneeAngle,
				RearRightKneeSpeed,
				RearLeftHipAngle,
				RearLeftHipSpeed,
				RearRightHipAngle,
				RearRightHipSpeed,
				ForeLeftKneeAngle,
				ForeLeftKneeSpeed,
				ForeRightKneeAngle,
				ForeRightKneeSpeed,
				ForeLeftHipAngle,
				ForeLeftHipSpeed,
				ForeRightHipAngle,
				ForeRightHipSpeed,

				_Next,
			};

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
			// parameters? eg. scale, joint strength, speed, etc
		};

	public:
		test_quadruped_body(phys_agent_specification const& spec, phys_system* system);

	public:
		virtual agent_sensor_name_list get_sensors() const { return spec::sensor_inputs(); }
		virtual size_t get_num_effectors() const { return spec::num_effectors(); }

		virtual double get_sensor_value(agent_sensor_id const& sensor) const;
		virtual void activate_effectors(std::vector< double > const& activations);

	public:
		float m_torso_len;
		float m_torso_width;
		float m_upper_len;
		float m_upper_width;
		float m_lower_len;
		float m_lower_width;

		b2Body* torso;
		b2Body* rear_upper1;
		b2Body* rear_upper2;
		b2Body* rear_lower1;
		b2Body* rear_lower2;
		b2Body* fore_upper1;
		b2Body* fore_upper2;
		b2Body* fore_lower1;
		b2Body* fore_lower2;
		b2RevoluteJoint* rear_hip1;
		b2RevoluteJoint* rear_hip2;
		b2RevoluteJoint* rear_knee1;
		b2RevoluteJoint* rear_knee2;
		b2RevoluteJoint* fore_hip1;
		b2RevoluteJoint* fore_hip2;
		b2RevoluteJoint* fore_knee1;
		b2RevoluteJoint* fore_knee2;
	};

}


#endif


