// control_fsm_defn.h

#ifndef __WB_GA_CLIENT_CONTROL_FSM_DEFN_H
#define __WB_GA_CLIENT_CONTROL_FSM_DEFN_H

#include "clsm/clsm.h"
#include "ddl/ddl.h"

#include "../ga_serializer.h"

#include "evo_database/evodb_session.h"

#include <boost/asio.hpp>
#include <boost/statechart/state_machine.hpp>

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <vector>


namespace ga {
	struct ga_defn;
}

namespace ga_control {
	namespace fsm {

		namespace sc = boost::statechart;
		namespace mpl = boost::mpl;
		namespace asio = boost::asio;

		struct root;

		struct ga_controller: clsm::state_machine < ga_controller, root >
		{
			ga_controller(
				ga::ga_defn& _ga_def,
				ddl::defn_node _ddl_defn,
				ga::i_ga_serializer* _serializer,
				evodb_session* _dbs,
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
			void set_seed(unsigned int seed);

			//std::map< std::string, std::shared_ptr< sys::i_system_defn > > sys_defns;
			ddl::defn_node ddl_defn;
			ddl::sd_tree ddl_data;

			ga::ga_defn& ga_def;
			ga::i_ga_serializer* serializer;
			evodb_session* db_session;

			unsigned int ga_rseed;

			asio::io_service io_service;
			std::unique_ptr< asio::io_service::work > io_work;

			std::function< void(std::string) > output_sink;
			std::function< void(std::string) > prompt_callback;
		};

	}
}


#endif


