// rtp_phys_scenario.h

#ifndef __NE_RTP_PHYS_SCENARIO_H
#define __NE_RTP_PHYS_SCENARIO_H

#include "rtp_phys_system.h"
#include "../rtp_scenario.h"
#include "../../rtp_param.h"
#include "../../params/enum_par.h"
#include "../../params/paramlist_par.h"

#include <boost/random/uniform_real_distribution.hpp>


struct rtp_named_param_list;

namespace Wt {
	class WPainter;
}

namespace rtp_phys
{
	class phys_scenario_base	// ?? i_scenario
	{
	public:
		enum Type {
			GroundBased,
			SpaceBased,

			Count,
		};

		static const std::string Names[Type::Count];
	};

	class phys_scenario: public phys_scenario_base
	{
	public:
		typedef phys_system system_t;
		typedef system_t::state state_t;
		typedef system_t::scenario_data scenario_data_t;

		class enum_param_type: public rtp_enum_param_type
		{
		public:
			enum_param_type();
		};

		class param_type: public rtp_paramlist_param_type
		{
		public:
			virtual size_t provide_num_child_params(rtp_param_manager* mgr) const;
			virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const;
		};

		static rtp_param_type* params();
//		static rtp_named_param_list params(Type scen);

		static phys_scenario* create_instance(rtp_param param);

	public:
		virtual boost::any generate_initial_state(rgen_t& rgen) const = 0;
		virtual void load_initial_state(boost::any const& data, phys_system::state& st) const = 0;
		virtual scenario_data_t get_scenario_data() const;
		virtual bool is_complete(state_t const& st);

		// TODO: Ideally we want a scenario/system to have a collection of i_world_object interface pointers,
		// which can draw themselves.
		virtual void draw_fixed_objects(Wt::WPainter& painter) const {}
		//

	public:
		int m_duration;
	};

}


#endif

