// control_fsm_defn.cpp

#include "control_fsm_defn.h"
#include "control_fsm.h"
#include "../ga_params.h"

#include "params/param_accessor.h"

#include <chrono>
#include <thread>


namespace ga_control {
	namespace fsm {

		ga_controller::ga_controller(
			ga::ga_defn& _ga_def,
			prm::param_tree _pt,
			prm::schema::schema_provider_map_handle _provider,
			ga::i_ga_serializer* _serializer,
			evodb_session* _dbs,
			std::function< void(std::string) > _output_sink,
			std::function< void(std::string) > _prompt_callback
			):
			ga_def{ _ga_def },
			ptree{ std::move(_pt) },
			provider{ _provider },
			serializer{ _serializer },
			db_session{ _dbs },
			output_sink{ _output_sink },
			prompt_callback{ _prompt_callback }
		{
			// TODO: default to what?
			// ALSO: Currently reset command will modify the seed, but if exit and then init, seed will remain
			// the same as previously...
			ga_rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
				// 0;
		}

		std::string ga_controller::get_prompt() const
		{
			return state_cast<clsm::i_cmd_state const&>().get_prompt();
		}

		void ga_controller::start()
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

		std::string ga_controller::post(std::function< void() > fn)
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
		void ga_controller::terminate()
		{
			io_work.reset();
		}

		bool ga_controller::is_defn_complete() const
		{
			auto tree_copy = ptree;
			auto acc = prm::param_accessor{ &tree_copy };
			// TODO:
			return true;// !prm::extract_as< std::string >(acc["target_str"]).empty();
		}

		void ga_controller::set_seed(unsigned int seed)
		{
			ga_rseed = seed;
		}

	}
}




