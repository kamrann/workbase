// system_sim_client.cpp : Defines the entry point for the console application.
//

#include "system_sim_client.h"
#include "control_fsm.h"

//#include "systems/elevator/elevator_system_defn.h"
#include "systems/physics2d/phys2d_system_defn.h"

#include "system_sim/system.h"

#include "evolvable_systems/controllers/db_evolved_controller.h"

#include "evo_database/evo_db.h"

#include "ddl/ddl.h"
#include "ddl/state_machine/paramtree_fsm.h"
//
#include "ddl/dependency_graph.h"
//

#include "wt_cmdline_server/wt_server.h"
#include "wt_displays/chart.h"
#include "wt_displays_ne/drawer.h"

#include <Wt/Dbo/FixedSqlConnectionPool>

#include <boost/program_options.hpp>

#include <map>
#include <memory>
#include <iostream>
#include <thread>


namespace po = boost::program_options;


// TODO: map of unique_ptr giving nonsense compiler error, despite same being fine in system_sim_pwex
std::map< std::string, std::shared_ptr< sys::i_system_defn > > sys_defns;	// unique_ptr<>


int _tmain(int argc, _TCHAR* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "show help message")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	//
	std::unique_ptr< Wt::Dbo::SqlConnectionPool > db_cp{
		evodb_session::create_connection_pool("host=127.0.0.1 user=Cameron password='' port=5432 dbname=evo")
		//"./evo.db")
	};
	if(!db_cp)
	{
		return -1;
	}
	//
	
	std::unique_ptr< sys::i_system_defn > defn;

//	defn = sys::elev::elevator_system_defn::create_default();
//	sys_defns[defn->get_name()] = std::shared_ptr < sys::i_system_defn > { defn.release() };//std::move(defn);

	defn = sys::phys2d::phys2d_system_defn::create_default();
	sys_defns[defn->get_name()] = std::shared_ptr < sys::i_system_defn > { defn.release() };//std::move(defn);

	auto db_cp_raw = db_cp.get();
	auto db_session_fn = [db_cp_raw]
	{
		return std::make_unique< evodb_session >(*db_cp_raw);
	};
	for(auto& df : sys_defns)
	{
		df.second->add_controller_defn("db", std::make_unique< sys::ev::db_evolved_controller_defn >(db_session_fn));
	}


	arg_list server_args{
		"system_sim_client.exe",
		"--docroot", ".",
		"--http-address", "0.0.0.0",
		"--http-port", "8080"
//		, "--accesslog", "NUL"
	};

	auto server_ok = start_wt_server(server_args);
	if(!server_ok)
	{
		std::cout << "Failed to start Wt server" << std::endl;
		return 0;
	}

	auto term = create_wt_terminal("system%20simulation%20terminal");

	auto sink = [term](std::string text)
	{
		term->output(text);
	};

	auto prompt_cb = [term](std::string prompt)
	{
		term->set_prompt(prompt);
	};


	register_display_defn(std::make_unique< chart_display_defn >());
	register_display_defn(std::make_unique< drawer_display_defn >());


	ddl::specifier spc;
	client_ddl schema;
	auto ddl_defn = schema.get_defn(spc, sys_defns);

	sys_control::fsm::system_controller sys_ctrl{
		sys_defns,// can't move as being global, it is accessed directly from the schema lambdas above... std::move(sys_defns),
		ddl_defn,
		sink,
		prompt_cb
	};

	term->register_command_handler([&sys_ctrl, term](std::string cmd)
	{
		// NOTE: This handler is invoked from a Wt session thread
		std::cout << "cmd [" << cmd << "]" << std::endl;

//		sys_ctrl.post([&sys_ctrl, cmd]
//		{
			sys_ctrl.process_event(clsm::ev_unprocessed_cmd_str{ cmd });
//		});

			if(sys_ctrl.terminated())
			{
				sys_ctrl.io_work.reset();
			}
			else
			{
				term->set_prompt(sys_ctrl.get_prompt());
			}
	});


	sys_ctrl.start();

	// ?? sys_ctrl.terminate();

	terminate_wt_server();

	return 0;

}

