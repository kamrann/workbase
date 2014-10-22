// control_fsm.cpp

#include "control_fsm.h"

#include "system_sim/system_defn.h"
#include "system_sim/system.h"

#include "params/param_accessor.h"

#include <chrono>
#include <thread>


namespace sys_control {
	namespace fsm {

		system_controller::system_controller(
			std::map< std::string, std::shared_ptr< sys::i_system_defn > > _sys_defns,
			prm::param_tree _pt,
			std::function< void(std::string) > _output_sink,
			std::function< void(std::string) > _prompt_callback
			):
			sys_defns{ std::move(_sys_defns) },
			ptree{ std::move(_pt) },
			output_sink{ _output_sink },
			prompt_callback{ _prompt_callback }
		{

		}

		void system_controller::start()
		{
			// Start the state machine
			initiate();

			//cmds_waiting = 0;
			io_work = std::make_unique< asio::io_service::work >(io_service);

			// temp
			io_service.post([this]
			{
				process_event(ev_init{});
			});
			//

			io_service.run();
		}

		std::string system_controller::post(std::function< void() > fn)
		{
			io_service.post([this, fn]
			{
				// Invoke command function
				fn();
			});

			return "";// state_cast<i_cmd_state const&>().get_prompt();
		}

		// TODO: inadvertently overrode statechart method
		/// need to look into how state machine should be exited
		void system_controller::terminate()
		{
			io_work.reset();
		}

		bool system_controller::is_defn_complete() const
		{
			return true;	// TODO:
		}
		/*
		bool system_controller::commands_waiting() const
		{
			return cmds_waiting > 0;
		}
		*/

		std::string root::get_prompt() const
		{
			return "@root>";
		}

		active::active(my_context ctx):
			my_base(ctx)
		{
			auto& sc_ctx = context< system_controller >();

			// Entering active system state, construct a system from the param tree
			auto acc = prm::param_accessor{ &sc_ctx.ptree };
			
			auto sys_type = prm::extract_as< prm::enum_param_val >(acc["sys_type"])[0];
			auto const& sys_defn = sc_ctx.sys_defns.at(sys_type);
			sys = sys_defn->create_system(acc);

			// And reset it
			sys_rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
			reset_system(boost::none);

			// Also store update state value ids
			auto update_value_names = prm::extract_as< prm::enum_param_val >(acc["updates"]);
			for(auto const& v : update_value_names)
			{
				update_vals.push_back(sys::state_value_id::from_string(v));
			}

			// And result state value ids
			auto result_value_names = prm::extract_as< prm::enum_param_val >(acc["results"]);
			for(auto const& v : result_value_names)
			{
				result_vals.push_back(sys::state_value_id::from_string(v));
			}
		}

		// Initialize the system
		void active::reset_system(boost::optional< unsigned int > rseed)
		{
			if(rseed)
			{
				sys_rseed = *rseed;
			}

			sys->set_random_seed(sys_rseed);
			sys->initialize();
			frame = 0;
			complete = false;
/*
			clear_all_chart_data();

			for(auto& drawer : m_drawers)
			{
				update_display(drawer.first);
			}
*/		}

		// Advances the system by a single update frame
		bool active::step_system(frame_update_cfg const& cfg)
		{
			auto still_going = sys->update(nullptr);

			if(cfg.output_updates)
			{
				output_state_values(update_vals);
			}

			++frame;

			return still_going;
		}

		bool active::step_system(size_t frames, frame_update_cfg const& cfg)
		{
			for(size_t i = 0; i < frames; ++i)
			{
				if(!step_system(cfg))
				{
					return false;
				}
			}
			return true;
		}

		void active::output_state_values(sys::state_value_id_list const& svids) const
		{
			// TODO: std::lock_guard< std::mutex > lock_{ m_sys_mx };

			for(auto v_id : svids)
			{
				auto bound = sys->get_state_value_binding(v_id);
				auto value = sys->get_state_value(bound);
				std::stringstream ss;
				ss << v_id.to_string() << ": " << value;
				context< system_controller >().output_sink(ss.str());
			}
		}

		sc::result active::react(ev_reset const& ev)
		{
			boost::optional< unsigned int > rseed;
			switch(ev.seed.method)
			{
				case reset_cmd::seed_data::SeedMethod::Random:
				rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
				break;

				case reset_cmd::seed_data::SeedMethod::Fixed:
				rseed = ev.seed.value;
				break;

				default:	// Same - leave rseed uninitialized so argument will be ignored and seed left unchanged
				break;
			}
			reset_system(rseed);

			return transit< not_running >();
		}

		std::string not_ready::get_prompt() const
		{
			return "@not_ready>";
		}

		std::string ready::get_prompt() const
		{
			return "@ready>";
		}

		sc::result not_running::react(ev_get const& ev)
		{
			// Send the requested system state values to the output sink
			context< active >().output_state_values(ev.svids);

			return discard_event();
		}

		std::string paused::get_prompt() const
		{
			return "@paused>";
		}

		sc::result paused::react(ev_step const& ev)
		{
			// Step system
			auto frames = ev.frames ? *ev.frames : 1;
			auto still_going = context< active >().step_system(frames);

			if(still_going)
			{
				// No transition
				return discard_event();
			}
			else
			{
				// Transition to completed state
				return transit< completed >();
			}
		}

		sc::result paused::react(ev_run const& ev)
		{
			//need to pass ev.frame_rate to initialization of running state...
			post_event(ev);
			return transit< running >();
		}
		
		completed::completed(my_context ctx): innermost_state_base(ctx)
		{
			auto& ctrl = context< system_controller >();
			ctrl.output_sink("system completed");

			// Output result values
			auto& atv = context< active >();
			if(!atv.result_vals.empty())
			{
				ctrl.output_sink("result state values:");
				atv.output_state_values(atv.result_vals);
			}
		}
		
		std::string completed::get_prompt() const
		{
			return "@completed>";
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

				still_going = context< active >().step_system(cfg);
			}

			if(!still_going)
			{
				context< active >().complete = true;
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
			auto& fsm_ctx = context< system_controller >();
			fsm_ctx.post([&fsm_ctx]
			{
				fsm_ctx.process_event(ev_pause{});
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

		sc::result running::react(ev_run const& ev)
		{
			run_framerate = ev.frame_rate;
			if(run_framerate && *run_framerate == 0.0)
			{
				// Set realtime frame rate for system
				run_framerate = 1.0;	// TODO:
			}

			interrupt_request = false;
			run_thread = std::thread{ [this]{ do_run(); } };
			
			return discard_event();
		}

/*		sc::result running::react(ev_resume_do_run const&)
		{
			context< system_controller >().io_service.post([this]
			{
				do_run();
			});

			return discard_event();
		}
*/		
	}
}




