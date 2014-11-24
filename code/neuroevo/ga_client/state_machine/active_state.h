// active_state.h

#ifndef __WB_GA_CLIENT_CONTROL_ACTIVE_STATE_H
#define __WB_GA_CLIENT_CONTROL_ACTIVE_STATE_H

#include "clsm/clsm.h"
#include "root_state.h"

#include "commands/reset_cmd_parser.h"
#include "commands/chart_cmd_parser.h"

#include "genetic_algorithm/genalg.h"
#include "genetic_algorithm/genetic_population.h"
#include "genetic_algorithm/problem_domain.h"

#include <boost/optional.hpp>

#include <mutex>


namespace ga_control {
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
			~active();

			void on_reset(clsm::ev_cmd< reset_cmd > const&);
			void on_chart(clsm::ev_cmd< chart_cmd > const& cmd);

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

			void reset_ga(boost::optional< unsigned int > rseed);
			bool do_generation(frame_update_cfg const& cfg = frame_update_cfg{});
			bool do_generations(size_t count, frame_update_cfg const& cfg = frame_update_cfg{});
			void add_chart();
			void clear_chart_data(unsigned long id);
			void clear_all_chart_data();
			void store_chart_series_data();
			void redraw_all_charts();

			std::unique_ptr< ga::genalg > alg;
			std::shared_ptr< ga::i_problem_domain > domain;
			std::mutex ga_mx;

			struct chart_defn
			{
/*				struct series_defn
				{
					sys::state_value_bound_id sv_bound_id;
				};

				std::vector< series_defn > series;
*/			};

			std::map< unsigned long, chart_defn > charts;
		};

	}
}


#endif


