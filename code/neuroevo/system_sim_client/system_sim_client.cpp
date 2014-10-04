// system_sim_client.cpp : Defines the entry point for the console application.
//

#include "systems/elevator/elevator_system_defn.h"
#include "systems/elevator/elevator_system.h"
#include "systems/elevator/dumb_elevator_controller.h"

#include "peerwork/pwork_client/local_peer.h"
#include "peerwork/shared/job_description.h"

#include "params/param_accessor.h"
#include "params/cmdline.h"
#include "params/qualified_path.h"
#include "params/schema_builder.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <thread>


namespace po = boost::program_options;
namespace sb = prm::schema;


int _tmain(int argc, _TCHAR* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "show help message")
		("peer", po::value< std::string >(), "peer location <hostname>:<port number>")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	auto client_name = std::string{ "system_sim_client" };

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


	auto defn = std::make_shared< sys::elev::elevator_system_defn >();
	defn->add_controller_defn("Preset", std::make_unique< sys::elev::dumb_elevator_controller_defn >());

	auto sch_mp = std::make_shared< sb::schema_provider_map >();
	prm::qualified_path root = std::string{ "root" };
	auto sys_root = defn->update_schema_providor(sch_mp, root);// prm::qualified_path{});

	(*sch_mp)[root + std::string{ "results" }] = [=](prm::param_accessor acc)
	{
		auto val_names = defn->get_system_state_values(acc);
		auto s = sb::enum_selection(
			"results",
			val_names,
			0,
			std::numeric_limits< int >::max()
			);
		return s;
	};

	(*sch_mp)[root] = [=](prm::param_accessor acc)
	{
		auto s = sb::list(root.leaf().name());
		sb::append(s, sch_mp->at(root + std::string{ sys_root })(acc));
		if(acc.is_available("floor_count") && !prm::is_unspecified(acc["floor_count"]))	// TODO: Hack
		{
			sb::append(s, sch_mp->at(root + std::string{ "results" })(acc));
		}
		return s;
	};

	auto dummy_acc = prm::param_accessor{};
	auto sch = (*sch_mp)[root](dummy_acc);
	auto pt = prm::param_tree::generate_from_schema(sch);


	// TODO: This setup probably not thread-safe
	std::thread io_t{ [&pr]{ pr->run(); } };


	prm::cmdline_processor cmdln{ sch_mp, pt };
	cmdln.enter_cmd_loop(std::cin, std::cout);

	pwork::job_description jd;
	jd.type = "syssim";
	auto yaml_spec = pt.convert_to_yaml();
	jd.specification = YAML::Dump(yaml_spec);

	std::cout << "Posting following simulation spec:" << std::endl;
	std::cout << jd.specification << std::endl;

	pr->post_job(jd);

	while(true)
	{
		std::string cmd;
		std::getline(std::cin, cmd);

		if(cmd == "q")
		{
			break;
		}
	}

	pr->disconnect();
	io_t.join();

	return 0;
}

