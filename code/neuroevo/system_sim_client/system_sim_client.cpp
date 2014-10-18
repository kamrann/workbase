// system_sim_client.cpp : Defines the entry point for the console application.
//

#include "system_sim_client.h"
//#include "sys_controller.h"
#include "control_fsm.h"
#include "sys_cmd_interface.h"
#include "cmd_parser.h"

#include "systems/elevator/elevator_system_defn.h"
#include "systems/physics2d/phys2d_system_defn.h"

#include "system_sim/system.h"

#include "params/param_accessor.h"
#include "params/cmdline.h"
#include "params/qualified_path.h"
#include "params/schema_builder.h"

#include "Wt Displays/wt_server.h"

#include <boost/program_options.hpp>

#include <map>
#include <memory>
#include <iostream>
#include <thread>


namespace po = boost::program_options;
namespace sb = prm::schema;


// TODO: map of unique_ptr giving nonsense compiler error, despite same being fine in system_sim_pwex
std::map< std::string, std::shared_ptr< sys::i_system_defn > > sys_defns;	// unique_ptr<>

bool run_system_command_loop(prm::param_accessor acc);


int _tmain(int argc, _TCHAR* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "show help message")
//		("peer", po::value< std::string >(), "peer location <hostname>:<port number>")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	auto client_name = std::string{ "system_sim_client" };
	/*
	if(!vm.count("peer"))
	{
		std::cout << "peer location must be specified" << std::endl;
		return 0;
	}

	auto peer_location = vm["peer"].as< std::string >();
	auto colon = peer_location.find(":");
	std::string hostname, port;
	if(colon != std::string::npos)
	{
		hostname = peer_location.substr(0, colon);
		port = peer_location.substr(colon + 1, std::string::npos);
	}
	else
	{
		std::cout << "usage: --peer <hostname>:<port number>..." << std::endl;
		return 0;
	}

	auto pr = pwork::local_peer::try_connect(client_name, hostname, port);
	if(!pr)
	{
		std::cout << "failed to connect to peer" << std::endl;
		return 0;
	}
	*/
	
	std::unique_ptr< sys::i_system_defn > defn;

	defn = sys::elev::elevator_system_defn::create_default();
	sys_defns[defn->get_name()] = std::shared_ptr < sys::i_system_defn > { defn.release() };//std::move(defn);

	defn = sys::phys2d::phys2d_system_defn::create_default();
	sys_defns[defn->get_name()] = std::shared_ptr < sys::i_system_defn > { defn.release() };//std::move(defn);

	auto sch_mp = std::make_shared< sb::schema_provider_map >();
	prm::qualified_path root = std::string{ "root" };

	(*sch_mp)[root + std::string{ "sys_type" }] = [=](prm::param_accessor acc)
	{
		auto val_names = std::vector < std::string > {};
		for(auto const& entry : sys_defns)
		{
			val_names.push_back(entry.first);
		}
		auto s = sb::enum_selection(
			"sys_type",
			val_names,
			0,
			1
			);
		return s;
	};

	for(auto const& entry : sys_defns)
	{
		auto const& name = entry.first;
		auto const& defn = entry.second;
		defn->update_schema_provider(sch_mp, root + name);
	}

	(*sch_mp)[root + std::string{ "results" }] = [=](prm::param_accessor acc)
	{
		auto sys_name = prm::extract_as< prm::enum_param_val >(acc["sys_type"])[0];
		auto const& defn = sys_defns.at(sys_name);
		auto sv_ids = defn->get_system_state_values(acc);
		auto val_names = std::vector < std::string > {};
		std::transform(
			std::begin(sv_ids),
			std::end(sv_ids),
			std::back_inserter(val_names),
			[](sys::state_value_id const& svid)
		{
			return svid.to_string();
		});

		auto s = sb::enum_selection(
			"results",
			val_names,
			0,
			std::numeric_limits< int >::max()
			);
		return s;
	};

	(*sch_mp)[root + std::string{ "updates" }] = [=](prm::param_accessor acc)
	{
		auto sys_name = prm::extract_as< prm::enum_param_val >(acc["sys_type"])[0];
		auto const& defn = sys_defns.at(sys_name);
		auto sv_ids = defn->get_system_state_values(acc);
		auto val_names = std::vector < std::string > {};
		std::transform(
			std::begin(sv_ids),
			std::end(sv_ids),
			std::back_inserter(val_names),
			[](sys::state_value_id const& svid)
		{
			return svid.to_string();
		});

		auto s = sb::enum_selection(
			"updates",
			val_names,
			0,
			std::numeric_limits< int >::max()
			);
		return s;
	};

	(*sch_mp)[root] = [=](prm::param_accessor acc)
	{
		auto s = sb::list(root.leaf().name());
		sb::append(s, sch_mp->at(root + std::string{ "sys_type" })(acc));
		if(acc.is_available("sys_type") && !prm::is_unspecified(acc["sys_type"]))
		{
			auto sys_name = prm::extract_as< prm::enum_param_val >(acc["sys_type"])[0];
			sb::append(s, sch_mp->at(root + sys_name)(acc));
			sb::append(s, sch_mp->at(root + std::string{ "results" })(acc));
			sb::append(s, sch_mp->at(root + std::string{ "updates" })(acc));
		}
		return s;
	};

	auto dummy_acc = prm::param_accessor{};
	auto sch = (*sch_mp)[root](dummy_acc);
	auto pt = prm::param_tree::generate_from_schema(sch, sch_mp);

	prm::cmdline_processor cmdln{ sch_mp, pt };
	cmdln.enter_cmd_loop(std::cin, std::cout);

	arg_list server_args{
		"system_sim_client.exe",
		"--docroot", ".",
		"--http-address", "0.0.0.0",
		"--http-port", "8080",
		"--accesslog", "NUL"
	};

	auto server_ok = start_wt_server(server_args);
	if(!server_ok)
	{
		std::cout << "Failed to start Wt server" << std::endl;
		return 0;
	}
	/*
	prm::param_accessor acc{ &pt };

	auto sys_type = prm::extract_as< prm::enum_param_val >(acc["sys_type"])[0];
	auto defn_it = sys_defns.find(sys_type);
	auto const& sys_defn = defn_it->second;

	auto system = sys_defn->create_system(acc);

	auto update_value_names = prm::extract_as< prm::enum_param_val >(acc["updates"]);
	sys::state_value_id_list update_vals;
	for(auto const& v : update_value_names)
	{
		update_vals.push_back(sys::state_value_id::from_string(v));
	}

	auto result_value_names = prm::extract_as< prm::enum_param_val >(acc["results"]);
	sys::state_value_id_list result_vals;
	for(auto const& v : result_value_names)
	{
		result_vals.push_back(sys::state_value_id::from_string(v));
	}

	sys_control::controller sys_ctrl{
		sys_defn,
		std::move(system),
		std::move(update_vals),
		std::move(result_vals),
		std::cout
	};

	*/

	sys_control::fsm::system_controller sys_ctrl{ std::move(sys_defns), std::move(pt), [](std::string prompt)
	{
		std::cout << std::endl << prompt;
	} };
	std::thread ctrl_thread{ [&sys_ctrl]
	{
		sys_ctrl.start();
	} };

	sys_control::sys_cmd_interface cmd_if{ sys_ctrl, std::cout };
	cmd_if.enter_cmd_loop(std::cin);

	sys_ctrl.terminate();

	terminate_wt_server();

	return 0;

}

