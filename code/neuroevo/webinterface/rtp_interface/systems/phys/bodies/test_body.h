// test_body.h

#ifndef __RTP_PHYS_TEST_BODY_H
#define __RTP_PHYS_TEST_BODY_H

#include "../rtp_phys_agent_body_spec.h"
#include "composite_rigid_body.h"


class b2RevoluteJoint;

namespace rtp {

	class phys_system;
	struct phys_agent_specification;

	class test_body: public composite_rigid_body
	{
	public:
		class spec: public agent_body_spec
		{
		public:
			enum Sensors {
				_Prev = composite_rigid_body::Sensors::_Next - 1,

				JointAngle,
				JointSpeed,

				_Next,
			};

		public:
			static prm::schema::schema_node get_schema(prm::param_accessor param_vals);
			static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
//			static i_agent_factory* create_factory(prm::param_accessor spec_acc, prm::param_accessor inst_acc);
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
			double max_joint_torque;

			friend class test_body;
		};

	public:
		test_body(phys_agent_specification const& spec, phys_system* system);

	public:
		virtual agent_sensor_name_list get_sensors() const { return spec::sensor_inputs(); }
		virtual size_t get_num_effectors() const { return spec::num_effectors(); }

		virtual double get_sensor_value(agent_sensor_id const& sensor) const;
		virtual void activate_effectors(std::vector< double > const& activations);

		double get_max_joint_torque() const;

	public:
		float m_rear_len;
		float m_fore_len;
		float m_width;

		double m_max_joint_torque;

		b2Body* fore;
		b2Body* rear;
		b2RevoluteJoint* joint;
	};

}


#endif


