// control_fsm_defn.h

#ifndef __WB_SYSSIM_CLIENT_CONTROL_FSM_DEFN_H
#define __WB_SYSSIM_CLIENT_CONTROL_FSM_DEFN_H

#include "clsm/clsm.h"
#include "system_sim/system_sim_fwd.h"

#include "ddl/ddl.h"

#include <boost/asio.hpp>
#include <boost/statechart/state_machine.hpp>

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <vector>


namespace sys_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;
		namespace asio = boost::asio;

		struct root;

		struct system_controller: clsm::state_machine < system_controller, root >
		{
			system_controller(
				std::map< std::string, std::shared_ptr< sys::i_system_defn > > _sys_defns,
				ddl::defn_node _ddl_defn,
				std::function< void(std::string) > _output_sink,
				// TEMP
				std::function< void(std::string) > _prompt_callback
				);

			// External querying
			std::string get_prompt() const;

			void start();
			std::string post(std::function< void() > fn);
			void terminate();

			bool is_defn_complete() const;

			std::map< std::string, std::shared_ptr< sys::i_system_defn > > sys_defns;
			ddl::defn_node ddl_defn;
//			ddl::value_node ddl_data;
			ddl::sd_tree ddl_data;

			asio::io_service io_service;
			std::unique_ptr< asio::io_service::work > io_work;

			std::function< void(std::string) > output_sink;
			std::function< void(std::string) > prompt_callback;
		};

	}
}


#endif


