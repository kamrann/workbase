// rtp_radial_detection_system.h

#ifndef __NE_RTP_RADIAL_DETECTION_SYSTEM_H
#define __NE_RTP_RADIAL_DETECTION_SYSTEM_H

#include "../rtp_system.h"
#include "../rtp_agent.h"

#include "wt_param_widgets/pw_fwd.h"

#include "util/dimensionality.h"

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include <string>
#include <array>
#include <vector>
#include <memory>
#include <random>


namespace rtp {

	class i_controller;
	class i_observer;
	class i_properties;
	class i_property_values;
	class i_system_drawer;

	struct rd_agent_specification
	{};

	class rd_system:
		public i_system,
		public i_agent
	{
	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable);

		static std::unique_ptr< i_system_factory > generate_factory(prm::param_accessor param_vals);
		static agents_data generate_agents_data(prm::param_accessor param_vals, bool evolvable);

		static std::vector< std::string > get_state_values(prm::param_accessor param_vals);
		static std::vector< std::string > get_agent_type_names();
		static std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals);
		static size_t get_agent_num_effectors(prm::param agent_type);

	public:
		rd_system(prm::param_accessor params);
		~rd_system();

	public:
		virtual bool is_instantaneous() const override;
		virtual update_info get_update_info() const override;

		virtual void initialize() override;
		virtual void clear_agents() override;
		virtual agent_id register_agent(agent_specification const& spec) override;
		virtual void register_agent_controller(agent_id agent, std::unique_ptr< i_controller > controller) override;
		
		virtual i_agent const& get_agent(agent_id id) const;
		virtual i_controller const& get_agent_controller(agent_id id) const;

		virtual bool update(i_observer* obs);
		virtual void register_interactive_input(interactive_input const& input);
		virtual i_observer::observations_t record_observations(i_observer* obs) const;

		virtual std::shared_ptr< i_properties const > get_state_properties() const;
		virtual std::shared_ptr< i_property_values const > get_state_property_values() const;
		virtual double get_state_value(state_value_id id) const override;
		virtual std::unique_ptr< i_system_drawer > get_drawer() const;

		//
		virtual std::string get_name() const override;
		virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
		virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

	public:
		enum DetectorType {
			SingleObjectDirectionXY,
			SingleObjectDirectionBearing,
			SingleObjectCollisionProbability,

			Count,
		};

	protected:
		typedef DimensionalityTraits< WorldDimensionality::dim2D > dim_traits;

		struct decision
		{
			double angle;
			//			double urgency;

			double collision_prob;
		};

		std::tuple< bool, double > line_circle_intersection(
			dim_traits::position_t const& x,
			dim_traits::direction_t const& d,
			dim_traits::position_t const& p,
			double r
			);
			
		void initialize_objects();
		void read_sensors();
		decision make_decision();

	public:
		size_t m_num_sensors;	// Equally spaced radially
		DetectorType m_detector_type;

		struct sensor_reading
		{
/*			Eigen::Vector2d rel_pos;	// Vector from sensor/agent center to detection point
			Eigen::Vector2d rel_vel;	// Velocity of detection point relative to sensor/agent center of mass

			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
*/
			bool positive;
			double distance;
			double radial_vel;
			double lateral_vel;
		};
		
//		std::vector< sensor_reading, Eigen::aligned_allocator< sensor_reading > > m_sensor_readings;
		std::vector< sensor_reading > m_sensor_readings;
		std::unique_ptr< i_controller > m_controller;

		struct object
		{
			Eigen::Vector2d position;
			double radius;
			Eigen::Vector2d velocity;

			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		};

		object m_object;
		double m_collision_prob;
		decision m_decision;
		
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		static std::vector< std::string > generate_state_value_names(size_t num_sensors);
		static std::map< std::string, agent_sensor_id > generate_state_value_idmap(size_t num_sensors);

		std::vector< std::string > m_state_value_names;
		std::map< std::string, agent_sensor_id > m_state_value_ids;

		void initialize_state_value_accessors();

		typedef std::function< double() > state_value_accessor_fn;
		std::vector< state_value_accessor_fn > m_state_value_accessors;

	private:

//#ifdef PERF_LOGGING
	public:
		std::chrono::high_resolution_clock::duration
			m_sensor_update_time,
			m_decision_update_time,
			m_observer_update_time;

		virtual void concatenate_performance_data(perf_data_t& pd) const override;
//#endif

		friend class rd_system_drawer;
	};

}


#endif

