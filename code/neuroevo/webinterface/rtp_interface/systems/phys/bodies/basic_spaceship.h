// basic_spaceship.h

#ifndef __RTP_PHYS_BASIC_SPACESHIP_H
#define __RTP_PHYS_BASIC_SPACESHIP_H

#include "../rtp_phys_agent_body_spec.h"
#include "simple_rigid_body.h"

#include "thrusters/thruster.h"


class b2Body;

namespace rtp {

	class phys_system;
	struct phys_sensor;
	struct phys_agent_specification;

	class basic_spaceship: public simple_rigid_body
	{
	public:
		class spec: public agent_body_spec
		{
		public:
			enum Sensors {
				_Prev = simple_rigid_body::Sensors::_Next - 1,

				FrontSensor,
				RearSensor,
				RightSensor,
				LeftSensor,
				
				Collisions,
				Damage,

				Thruster_1_Temp,
				Thruster_2_Temp,
				Thruster_3_Temp,
				Thruster_4_Temp,
				Thruster_5_Temp,
				Thruster_6_Temp,
				Thruster_7_Temp,
				Thruster_8_Temp,

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
		basic_spaceship(phys_agent_specification const& spec, phys_system* system);

	public:
		virtual agent_sensor_name_list get_sensors() const { return spec::sensor_inputs(); }
		virtual size_t get_num_effectors() const { return spec::num_effectors(); }

		virtual double get_sensor_value(agent_sensor_id const& sensor) const;
		virtual void activate_effectors(std::vector< double > const& activations);

		virtual void on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type) override;
		virtual void on_collision(b2Fixture* fixA, b2Fixture* fixB, double approach_speed) override;

		virtual void draw(Wt::WPainter& painter) const;

	public:
		boost::shared_ptr< thruster_config< WorldDimensionality::dim2D > > m_t_cfg;
		thruster_system< WorldDimensionality::dim2D > m_t_system;

		float m_half_width;
		double m_mass;
		double m_rotational_inertia;
		double m_thruster_strength;
		double m_sensor_range;

		std::unique_ptr< phys_sensor > m_front_sensor;
		std::unique_ptr< phys_sensor > m_rear_sensor;
		std::unique_ptr< phys_sensor > m_left_sensor;
		std::unique_ptr< phys_sensor > m_right_sensor;

		size_t m_collisions;
		double m_damage;
	};

}


#endif


