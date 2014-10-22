// control_fsm_defn.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_FSM_DEFN_H
#define __WB_SYSSIM_CLIENT_CONTROL_FSM_DEFN_H

#include "system_sim/system_sim_fwd.h"

#include "params/param_tree.h"

#include <boost/asio.hpp>
#include <boost/statechart/state_machine.hpp>

#include <string>
#include <memory>
#include <functional>
#include <map>


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;
		namespace asio = boost::asio;

		struct root;
		struct system_controller: sc::state_machine < system_controller, root >
		{
			system_controller(
				std::map< std::string, std::shared_ptr< sys::i_system_defn > > _sys_defns,
				prm::param_tree _pt,
				std::function< void(std::string) > _output_sink,
				// TEMP
				std::function< void(std::string) > _prompt_callback
				);

			void start();
			std::string post(std::function< void() > fn);
			void terminate();

			bool is_defn_complete() const;

			std::map< std::string, std::shared_ptr< sys::i_system_defn > > sys_defns;
			prm::param_tree ptree;

			asio::io_service io_service;
			std::unique_ptr< asio::io_service::work > io_work;

			std::function< void(std::string) > output_sink;
			std::function< void(std::string) > prompt_callback;
		};

	}
}


#endif


