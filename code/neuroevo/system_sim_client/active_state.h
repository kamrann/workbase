// active_state.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_ACTIVE_STATE_H
#define __WB_SYSSIM_CLIENT_CONTROL_ACTIVE_STATE_H

#include "clsm.h"
#include "root_state.h"
#include "system_sim/system_state_values.h"

#include <boost/optional.hpp>

#include <mutex>


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;

		struct not_running;
		struct running;
		struct active:
			clsm::clsm_state_base < active, root, not_running >,
			clsm::i_cmd_state
		{
			typedef mpl::list<
				sc::transition< ev_exit, ready >
				, sc::custom_reaction< ev_reset >
			> reactions;

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

			active(my_context ctx);

			void reset_system(boost::optional< unsigned int > rseed);
			bool step_system(frame_update_cfg const& cfg = frame_update_cfg{});
			bool step_system(size_t frames, frame_update_cfg const& cfg = frame_update_cfg{});
			void output_state_values(sys::state_value_id_list const& svids) const;

			sc::result react(ev_reset const&);

			unsigned int sys_rseed;
			sys::system_ptr sys;
			std::mutex sys_mx;
			sys::state_value_id_list update_vals;
			sys::state_value_id_list result_vals;
			size_t frame;
			bool complete;
		};

	}
}


#endif


