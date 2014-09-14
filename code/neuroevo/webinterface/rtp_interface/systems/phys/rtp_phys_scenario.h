// rtp_phys_scenario.h

#ifndef __NE_RTP_PHYS_SCENARIO_H
#define __NE_RTP_PHYS_SCENARIO_H

#include "rtp_phys_system.h"
#include "../rtp_scenario.h"

#include <boost/random/uniform_real_distribution.hpp>


namespace Wt {
	class WPainter;
}

namespace rtp
{
	class phys_scenario_base
	{
	public:
		enum Type {
			GroundBased,
			SpaceBased,
			Target,
			Obstacles,

			Count,
			Default = GroundBased,
		};

		static const std::string Names[Type::Count];
	};

	class phys_scenario:
		public phys_scenario_base
	{
	public:
		typedef phys_system system_t;
		typedef system_t::state state_t;
		typedef system_t::scenario_data scenario_data_t;

		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		static std::unique_ptr< phys_scenario > create_instance(prm::param_accessor param);
		static std::vector< std::string > get_state_values(prm::param_accessor param_vals);

	public:
		virtual boost::any generate_initial_state(rgen_t& rgen) const = 0;
		virtual void load_initial_state(boost::any const& data, b2World* world/*phys_system::state& st*/) = 0;
		virtual scenario_data_t get_scenario_data() const;
		virtual bool is_complete(state_t const& st);

		// TODO: Ideally we want a scenario/system to have a collection of i_world_object interface pointers,
		// which can draw themselves.
		virtual void draw_fixed_objects(Wt::WPainter& painter) const {}
		//

		virtual double get_state_value(state_value_id id) const { return 0.0; }

	public:
		void BeginContact(b2Contact* contact) {}
		void EndContact(b2Contact* contact) {}

	public:
		int m_duration;
	};

}


#endif

