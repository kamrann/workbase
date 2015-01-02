// active_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_ACTIVE_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_ACTIVE_STATE_H

#include "clsm/clsm.h"
#include "root_state.h"

#include "reset_cmd_parser.h"
#include "chart_cmd_parser.h"
#include "drawer_cmd_parser.h"

#include "system_sim/system.h"
#include "system_sim/system_state_values.h"

#include <boost/optional.hpp>

#include <mutex>


namespace input {
	class input_stream;
}

namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct not_running;
		struct ready;

		struct active:
			clsm::clsm_state_base < active, root, not_running >
		{
			typedef clsm_state_base base;

			struct exit_cmd{};

			active(my_context ctx);

			void on_reset(clsm::ev_cmd< reset_cmd > const&);
			void on_chart(clsm::ev_cmd< chart_cmd > const& cmd);
			void on_drawer(clsm::ev_cmd< drawer_cmd > const& cmd);

			sc::result react(clsm::ev_cmd< reset_cmd > const& cmd);

			typedef mpl::list<
				sc::transition< clsm::ev_cmd< exit_cmd >, ready >
				, sc::custom_reaction< clsm::ev_cmd< reset_cmd > >
			> my_reactions;

			typedef mpl::copy<
				my_reactions,
				mpl::inserter< base::reactions, mpl::insert< mpl::_1, mpl::end< mpl::_1 >, mpl::_2 > >
			>::type reactions;

			using base::react;


			struct frame_update_cfg
			{
				bool output_updates;
				bool store_chart_series_data;
				bool redraw_charts;
				bool redraw_system;

				frame_update_cfg():
					output_updates{ true },
					store_chart_series_data{ true },
					redraw_charts{ true },
					redraw_system{ true }
				{}
			};

			void reset_system(boost::optional< unsigned int > rseed);
			bool step_system(frame_update_cfg const& cfg = frame_update_cfg{});
			bool step_system(size_t frames, frame_update_cfg const& cfg = frame_update_cfg{});
			void output_state_values(sys::state_value_id_list const& svids);
			void add_chart(sys::state_value_id_list vals);
			void add_drawer(double zoom);
			void clear_chart_data(unsigned long id);
			void clear_all_chart_data();
			void store_chart_series_data();
			void redraw_all_charts();
			void redraw_all_system_views();

			unsigned int sys_rseed;
			sys::system_ptr sys;
			std::mutex sys_mx;
			sys::state_value_id_list update_vals;
			sys::state_value_id_list result_vals;
			size_t frame;
			bool complete;

			std::shared_ptr< input::input_stream > in_strm;

			struct chart_defn
			{
				struct series_defn
				{
					sys::state_value_bound_id sv_bound_id;
				};

				std::vector< series_defn > series;
			};

			std::map< unsigned long, chart_defn > charts;

			struct drawer_defn
			{};

			std::map< unsigned long, drawer_defn > drawers;
		};

	}
}


#endif


