// control_fsm_defn.cpp

#include "control_fsm_defn.h"

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

	}
}




