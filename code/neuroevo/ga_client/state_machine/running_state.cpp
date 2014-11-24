// running_state.cpp

#include "running_state.h"
#include "control_fsm.h"


namespace ga_control {
	namespace fsm {

		running::running(my_context ctx):
			base(ctx)
		{
			reg_cmd< stop_cmd >(wrap_grammar< stop_cmd_parser< clsm::iter_t > >());
		}

		running::~running()
		{
			interrupt_request = true;
			run_thread.join();
		}

		void running::do_run()
		{
			active::frame_update_cfg cfg;
			if(!run_framerate)
			{
				// Disable redraws for continuous update
				cfg.redraw_charts = false;
				cfg.redraw_system = false;
			}

			bool still_going = true;
			while(still_going && !interrupt_requested())
			{
				if(run_framerate)
				{
					auto dur = std::chrono::duration < double > { 1.0 / *run_framerate };
					std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(dur));
				}

				still_going = context< active >().do_generation(cfg);
			}

			if(!still_going)
			{
//				context< active >().complete = true;
			}

			/*			redraw_all_charts();
			redraw_all_system_views();

			if(!active)
			{
			notify_completion();
			}
			*/
			// Trigger a transition to the not_running state.
			// Note that in the case that we were interrupted due to an externally posted ev_pause event,
			// the transition will already be in process, and be completed when the following event is
			// processed. Since we will then be in the not_running state, this duplicate ev_pause will just
			// be ignored.
			auto& fsm_ctx = context< ga_controller >();
			fsm_ctx.post([&fsm_ctx]
			{
				fsm_ctx.process_event(clsm::ev_cmd< stop_cmd >{});
			});

			std::cout << "exiting run thread (system ";
			if(still_going)
			{
				std::cout << "still going";
			}
			else
			{
				std::cout << "completed";
			}
			std::cout << ")" << std::endl;
		}

		bool running::interrupt_requested() const
		{
			return interrupt_request;
		}

		std::string running::get_prompt() const
		{
			return "@running>";
		}

		void running::on_run(clsm::ev_cmd< run_cmd > const& cmd)
		{
			run_framerate = cmd.frame_rate;
			if(run_framerate && *run_framerate == 0.0)
			{
				// Set realtime frame rate for system
				run_framerate = 1.0;	// TODO:
			}

			interrupt_request = false;
			run_thread = std::thread{ [this]{ do_run(); } };
		}

	}
}




