// obstacles.h

#ifndef __NE_RTP_PHYS_OBSTACLES_SCENARIO_H
#define __NE_RTP_PHYS_OBSTACLES_SCENARIO_H

#include "../rtp_phys_scenario.h"


namespace prm {
	class param_accessor;
}

namespace rtp
{
	class obstacles_scenario: public phys_scenario
	{
	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		static std::vector< std::string > get_state_values(prm::param_accessor param_vals);

	public:
		static unsigned long const ObstacleId = 0xFFFFul;

		obstacles_scenario(prm::param_accessor param_vals);

	public:
		virtual boost::any generate_initial_state(rgen_t& rgen) const;
		virtual void load_initial_state(boost::any const& data, b2World* world);
		virtual bool is_complete(state_t const& st) override;
		virtual void draw_fixed_objects(Wt::WPainter& painter) const;

		virtual double get_state_value(state_value_id id) const;

	public:
//		virtual void BeginContact(b2Contact* contact) override;
//		virtual void EndContact(b2Contact* contact) override;

	protected:
		struct init_state_data
		{
			struct obstacle
			{
				//double radius;
				size_t vertex_count;
				std::array< b2Vec2, b2_maxPolygonVertices > vertices;

				b2Vec2 pos;
				double angle;
				b2Vec2 vel;
				double angvel;
			};

			std::vector< obstacle > obstacles;
		};

	protected:
		double m_x_expanse;
		double m_y_expanse;
		double m_x_center;
		double m_y_center;
		double m_density;
		double m_min_size;
		double m_max_size;
		double m_max_elongation;
		double m_max_speed;
		double m_max_angular_speed;
		double m_freezone_x;
		double m_freezone_y;
		double m_freezone_radius;

		std::vector< b2Body* > m_obstacles;
//		unsigned long m_collisions;
	};
}


#endif

