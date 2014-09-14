// target.h

#ifndef __NE_RTP_PHYS_TARGET_SCENARIO_H
#define __NE_RTP_PHYS_TARGET_SCENARIO_H

#include "../rtp_phys_scenario.h"


namespace prm {
	class param_accessor;
}

namespace rtp
{
	class target_scenario: public phys_scenario
	{
	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);

	public:
		enum {
			// TODO: parameter
			GroundSize = 100,
		};

		enum Params {

			Count,
		};

		static unsigned long const ObstacleId = 0xFFFFul;

		target_scenario(prm::param_accessor param_vals);

	public:
		virtual boost::any generate_initial_state(rgen_t& rgen) const;
		virtual void load_initial_state(boost::any const& data, b2World* world);
		virtual bool is_complete(state_t const& st) override;
		virtual void draw_fixed_objects(Wt::WPainter& painter) const;

	public:
		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);

	protected:
		struct init_state_data
		{
			b2Vec2 target_pos;

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
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_target_start_distance;
		
		b2Body* m_target;
		std::vector< b2Body* > m_obstacles;
		bool m_collided;
	};
}


#endif

