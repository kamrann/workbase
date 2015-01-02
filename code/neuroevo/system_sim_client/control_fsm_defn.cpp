// control_fsm_defn.cpp

#include "control_fsm_defn.h"
#include "control_fsm.h"

#include "system_sim/system_defn.h"
#include "system_sim/system.h"

#include <chrono>
#include <thread>


namespace sys_control {
	namespace fsm {

		system_controller::system_controller(
			std::map< std::string, std::shared_ptr< sys::i_system_defn > > _sys_defns,
			ddl::defn_node _ddl_defn,
			std::function< void(std::string) > _output_sink,
			std::function< void(std::string) > _prompt_callback
			):
			sys_defns{ std::move(_sys_defns) },
			ddl_defn{ _ddl_defn },
			output_sink{ _output_sink },
			prompt_callback{ _prompt_callback }
		{
			auto root_attribs = ddl::sd_node_attribs{};
			root_attribs.defn = ddl_defn;
			ddl_data.create_root(root_attribs);
		}

		std::string system_controller::get_prompt() const
		{
			return state_cast<clsm::i_cmd_state const&>().get_prompt();
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
//				process_event(clsm::ev_cmd< init_cmd >{});
			});
			//

			prompt_callback(get_prompt());

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




