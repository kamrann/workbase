// ga_client.cpp : Defines the entry point for the console application.
//

#include "ga_params.h"
#include "ga_db_serializer.h"
#include "plot_query.h"
#include "state_machine/control_fsm.h"

#include "params/schema_builder.h"
#include "params/state_machine/paramtree_fsm.h"

#include "wt_cmdline_server/wt_server.h"
#include "wt_displays/chart.h"
#include "wt_displays_ne/drawer.h"

#include <Wt/Dbo/FixedSqlConnectionPool>

#include <boost/program_options.hpp>


namespace po = boost::program_options;
namespace sb = prm::schema;


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

	auto db_session = std::make_unique< evodb_session >(*db_cp);
	//

	auto sch_mp = std::make_shared< sb::schema_provider_map >();
	prm::qualified_path root = std::string{ "root" };
	ga::ga_defn defn;
	defn.update_schema_provider(sch_mp, root);

	arg_list server_args{
		"ga_client.exe",
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

	auto term = create_wt_terminal("genetic%20algorithm%20terminal");

	auto sink = [term](std::string text)
	{
		term->output(text);
	};

	auto prompt_cb = [term](std::string prompt)
	{
		term->set_prompt(prompt);
	};


	register_display_defn(std::make_unique< chart_display_defn >());
//	register_display_defn(std::make_unique< drawer_display_defn >());


	auto dummy_acc = prm::param_accessor{};
	auto sch = (*sch_mp)[root](dummy_acc);
	auto pt = prm::param_tree::generate_from_schema(sch, sch_mp);

	ga::ga_db_serializer serializer(*db_session);
	serializer.set_options(ga::i_ga_serializer::Generation::StoreBest);

	ga_control::fsm::ga_controller ga_ctrl{
		defn,
		std::move(pt),
		sch_mp,
		&serializer,
		db_session.get(),
		sink,
		prompt_cb
	};

	term->register_command_handler([&ga_ctrl, term](std::string cmd)
	{
		// NOTE: This handler is invoked from a Wt session thread
		std::cout << "cmd [" << cmd << "]" << std::endl;

		//		sys_ctrl.post([&sys_ctrl, cmd]
		//		{
		ga_ctrl.process_event(clsm::ev_unprocessed_cmd_str{ cmd });
		//		});

		if(ga_ctrl.terminated())
		{
			ga_ctrl.io_work.reset();
		}
		else
		{
			term->set_prompt(ga_ctrl.get_prompt());
		}
	});


	ga_ctrl.start();

	terminate_wt_server();
	return 0;
}

