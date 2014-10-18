// control_fsm.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_FSM_H
#define __WB_SYSSIM_CLIENT_CONTROL_FSM_H

#include "cmd_parser.h"

#include "system_sim/system_sim_fwd.h"
#include "system_sim/system_state_values.h"

#include "params/param_tree.h"

#include <boost/optional.hpp>
#include <boost/asio.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/termination.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/mpl/list.hpp>

#include <string>
#include <memory>
#include <thread>
#include <mutex>


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;
		namespace asio = boost::asio;

		//////////////
		// See http://www.boost.org/doc/libs/1_56_0/libs/statechart/doc/rationale.html#DynamicConfigurability

		struct make_choice: sc::event< make_choice > {};

		template < class MostDerived, class Context >
		struct choice_point: sc::state< MostDerived, Context >
		{
			typedef sc::state< MostDerived, Context > base_type;
			typedef typename base_type::my_context my_context;
			typedef choice_point my_base;

			typedef sc::custom_reaction< make_choice > reactions;

			choice_point(my_context ctx): base_type(ctx)
			{
				this->post_event(boost::intrusive_ptr< make_choice >(new make_choice()));
			}
		};
		//////////////

		struct ev_quit: sc::event < ev_quit > {};
		struct ev_init: sc::event < ev_init > {};
		struct ev_exit: sc::event < ev_exit > {};
		struct ev_reset:
			sc::event < ev_reset >,
			reset_cmd
		{};
		struct ev_step: sc::event < ev_step > {};
		struct ev_run:
			sc::event < ev_run >,
			run_cmd
		{};
		struct ev_pause: sc::event < ev_pause > {};
		struct ev_get: sc::event < ev_get >
		{
			ev_get(sys::state_value_id_list _ids): svids{ _ids }
			{}

			sys::state_value_id_list svids;
		};


		struct i_cmd_state
		{
			virtual std::string get_prompt() const //= 0;
			{
				return "@somewhere>";
			}
		};

		struct root;
		struct system_controller: sc::state_machine < system_controller, root >
		{
			system_controller(
				std::map< std::string, std::shared_ptr< sys::i_system_defn > > _sys_defns,
				prm::param_tree _pt,
				// TEMP
				std::function< void(std::string) > _prompt_callback
				);

			void start();
			std::string post(std::function< void() > fn);
			void terminate();

			bool is_defn_complete() const;
//			bool commands_waiting() const;

			std::map< std::string, std::shared_ptr< sys::i_system_defn > > sys_defns;
			prm::param_tree ptree;

			asio::io_service io_service;
			std::unique_ptr< asio::io_service::work > io_work;
//			size_t cmds_waiting;
//			std::mutex cmd_mx;

			std::function< void(std::string) > prompt_callback;
		};

		struct inactive;
		struct active;
		struct root:
			sc::simple_state < root, system_controller, inactive >,
			i_cmd_state
		{
			typedef sc::termination< ev_quit > reactions;

			virtual std::string get_prompt() const override;
		};

		struct not_ready;
		struct ready;
		struct is_ready_choice;
		struct inactive:
			sc::simple_state< inactive, root, is_ready_choice >,
			i_cmd_state
		{};

		struct not_ready:
			sc::simple_state< not_ready, inactive >,
			i_cmd_state
		{
			virtual std::string get_prompt() const override;
		};

		struct ready:
			sc::simple_state< ready, inactive >,
			i_cmd_state
		{
			typedef sc::transition< ev_init, active > reactions;

			virtual std::string get_prompt() const override;
		};

		struct is_ready_choice: choice_point< is_ready_choice, inactive >
		{
			is_ready_choice(my_context ctx): my_base(ctx) {}

			sc::result react(const make_choice &)
			{
				if(context< system_controller >().is_defn_complete())
				{
					return transit< ready >();
				}
				else
				{
					return transit< not_ready >();
				}
			}
		};

		struct not_running;
		struct running;
		struct active:
			sc::state < active, root, not_running >,
			i_cmd_state
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
			bool step_system();
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

		struct paused;
		struct completed;
		struct is_complete_choice;
		struct not_running:
			sc::simple_state< not_running, active, is_complete_choice >,
			i_cmd_state
		{
			typedef mpl::list<
				sc::custom_reaction< ev_get >
			> reactions;

			sc::result react(ev_get const&);
		};

		struct paused:
			sc::simple_state < paused, not_running >,
			i_cmd_state
		{
			typedef mpl::list<
				sc::custom_reaction< ev_step >
				, sc::custom_reaction< ev_run >
			> reactions;

			virtual std::string get_prompt() const override;

			sc::result react(ev_step const&);
			sc::result react(ev_run const&);
		};

		struct completed:
			sc::state < completed, not_running >,
			i_cmd_state
		{
			completed(my_context ctx);

			virtual std::string get_prompt() const override;
		};

		struct is_complete_choice: choice_point< is_complete_choice, not_running >
		{
			is_complete_choice(my_context ctx): my_base(ctx) {}

			sc::result react(const make_choice &)
			{
				if(context< active >().complete)
				{
					return transit< completed >();
				}
				else
				{
					return transit< paused >();
				}
			}
		};

		struct running:
			sc::simple_state< running, active >,
			i_cmd_state
		{
			typedef mpl::list<
				sc::custom_reaction< ev_run >
				, sc::transition< ev_pause, not_running >
//				, sc::custom_reaction< ev_resume_do_run >
			> reactions;

			~running();

			void do_run();
			bool interrupt_requested() const;

			virtual std::string get_prompt() const override;

//			sc::result react(ev_pause const&);
			sc::result react(ev_run const&);
//			sc::result react(ev_resume_do_run const&);

			bool interrupt_request;
			boost::optional< double > run_framerate;
			std::thread run_thread;
		};
	}
}


#endif


